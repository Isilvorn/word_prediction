/*
** Created by: Jason Orender
** (c) 2018 all rights reserved
**
** This library implements a sparse vector format using the list standard container.  The list
** container was chosen so that new elements could be added or subtracted with minimal cost,
** since this will happen quite often as elements become zeroes and need to be eliminated from
** explicit expression in the vector.
*/

#include "../include/vect.h"

using namespace std;

/*
******************************************************************************
******************** Svect CLASS DEFINITION BELOW HERE ***********************
******************************************************************************
*/

/*
** Overloading the "<<" operator allows outputing the elements to an output stream.
*/

ostream& operator<<(ostream& os, const Svect& v) {
  os << "[ ";
  for (int i=0; i<v.size(); i++) os << v.element(i) << " ";
  os << "]";
  return os;
}


/*
** Overloading the ">>" operator allows inputing the elements from an input stream.
*/

istream& operator>>(istream& is, Svect& v) {
  double d;
  for (int i=0; i<v.size(); i++) {
    is >> d;                           // getting the input value
    if (d != 0.0) v.element_c(i) =  d; // only explicitly add the value if it is nonzero
  }
  return is;
}

/*
** Default constructor.
*/
Svect::Svect() { resize(0); }

/*
** Alternate constructor.
*/
Svect::Svect(int n) { resize(n); }

/*
** Copy constructor.
*/
Svect::Svect(const Svect &v) { copy(v); }

Svect::~Svect(void) {
  // explicitly erasing the list holding the data before destroying the object
  a.erase(a.begin(),a.end());
 }

/*
** The is_explicit() function returns whether the element is explicitly present in the
** list.  It will only be explicitly present if the value is nonzero.
*/
bool Svect::is_explicit(int n) const {
  list<Datapoint>::const_iterator it;
  it = a.begin();
  while (it != a.end()) { if ((*it).i == n) return true; it++; } // return true if the index is found
  return false;                                                  // return false if it is not
}

/*
** The count_explicit() function returns the number of elements that are explicitly 
** present in the list.
*/
int Svect::count_explicit(void) const { return a.size(); }

/*
** The remove() function removes an explicit element from the list, which essentially
** sets it to zero since when that element is referenced in the future a zero will
** be returned. If the element doesn't exist, it does nothing.
*/
void Svect::remove(int n) {
  int i=0;
  list<Datapoint>::iterator it;

  if (cache_index[n%CSZ] == n) { 
    a.erase(cache_it[n%CSZ]); 
    cache_index[n%CSZ] = -1; 
    return; 
  } // end if (cache_index)
  else {
    it = a.begin();
    while (it != a.end()) { 
      if ((*it).i == n) { a.erase(it); return; }
      it++; 
    } // end while (it)

  } // end else (cache_index)

} // end remove()

// this version takes an iterator as input
list<Datapoint>::iterator Svect::remove(list<Datapoint>::iterator &it) {
  if (cache_index[(*it).i%CSZ] == (*it).i) cache_index[(*it).i%CSZ] = -1;
  return a.erase(it);
}

/*
** The element() function extracts an element of a specific ordinal from the list.  It
** is meant to simulate the behavior of an array from the user perspective.
*/
double Svect::element(int n) const { 
  list<Datapoint>::const_iterator it;

  if (cache_index[n%CSZ] == n) return (*cache_dp[n%CSZ]);
  else {
    it = a.begin();
    while (it != a.end()) { 
      if ((*it).i == n) {
	return (*it).d; 
      } // end if (it)
      it++; 
    } // end while (it)
  } // end else (cache_index)

  return 0.0; // always returns zero if a corresponding index was not found
} // end element()

/*
** The element_c() function is like the element() function, but it creates a list entry
** if one was not found and passes back the reference.
*/
double& Svect::element_c(int n) { 
  list<Datapoint>::iterator it;
  Datapoint                 dp;

  if (cache_index[n%CSZ] == n) return (*cache_dp[n%CSZ]);
  else {
    if (cache_index[n%CSZ] != -1) {
      it = a.begin();
      while (it != a.end()) {  
	if ((*it).i == n) {
	  cache_index[n%CSZ] = n;
	  cache_dp[n%CSZ]    = &((*it).d);
	  cache_it[n%CSZ]    = it;
	  return (*it).d;
	} // end if (it)
	it++; 
      } // end while (it)
    } // end if (cache_index)

    dp.i = n;
    a.push_front(dp);
    cache_it[n%CSZ]    = a.begin();
    cache_index[n%CSZ] = n;
    cache_dp[n%CSZ]    = &((*a.begin()).d);
    return ((*a.begin()).d); // returns a new element if the index was not found

  } // end else (cache_index)

} // end element_c()

/*
** The setall() function sets every element explicitly to an input value. Note that this eliminates
** sparsity.
*/
void Svect::setall(double d) {
  Datapoint dp;
  resize(sz);           // the easiest way is to simply start from scratch since none of the data
                        // will be retained

  if (d != 0) {         // this only needs to be done if the input value is nonzero
    dp.d = d;
    for (int i=0; i<sz; i++) {
      dp.i = i;
      a.push_front(dp); // need to create a new element for each entry (eliminates sparsity)
      cache_it[i%CSZ]    = a.begin();
      cache_index[i%CSZ] = i;
      cache_dp[i%CSZ]    = &((*a.begin()).d);
    }
  }
}

/*
** The sete() function sets a specific element to an input value.  If the subscript is out of range,
** it does nothing.
*/
void Svect::sete(int i,double d) { if (i < sz) element_c(i) = d; }

/*
** This version of set uses an iterator instead of an index.
*/
void Svect::sete(list<Datapoint>::iterator &it, double d) {
  (*it).d = d;
  int i = (*it).i;
  cache_index[i%CSZ] = i;
  cache_dp[i%CSZ]    = &((*it).d);
  cache_it[i%CSZ]    = it;
}


/*
** The size() function returns the nominal size of the vector.
*/
int Svect::size(void) const { return sz; }

/*
** The "*=" operator when used with two vectors multiplies each of the vectors
** together element-by-element.  This does not correspond to a true matrix multiplication.
** If the vectors are not of equal size, it does nothing.
*/
Svect& Svect::operator*=(const Svect &v) {
  double d;
  int    i;
  list<Datapoint>::iterator it;

  if (v.size() == sz) {
    if (sz < CSZ) { // all values should be cached if this is true
      for (i=0; i<sz; i++) { if (cache_index[i] != -1) *cache_dp[i] *= v[i]; }
    } // end if (sz)
    else {
      it = a.begin();
      while (it != a.end()) {
	i = (*it).i;
	d = (*it).d * v[i];
	if (d != 0.0) { sete(it,d); it++; }
	else            it = remove(it);
      } // end while(it)
    } // end else (sz)
  } // end if (v)

  return *this;
}

/*
** This version of the "*=" unary operator simply multiplies every element in the
** vector by a constant.
*/
Svect& Svect::operator*=(const double f) {
  list<Datapoint>::iterator it;
  double d;

  if (f != 0.0) {
    if (sz < CSZ) { // all values should be cached if this is true
      for (int i=0; i<sz; i++)  { if (cache_index[i] != -1) *cache_dp[i] *= f; }
    } // end if (sz)
    else {
      it = a.begin();
      while (it != a.end()) { 
	d = (*it).d * f;
	sete(it, d); 
	it++; 
      }
    } // end else (sz)
  } // end if (f)
  else resize(sz); // this is the same as removing all explicit elements

  return *this;
} // end "*=" operator definition

/*
** This version of the "*" operator multiplies a vector by a constant.
*/
Svect Svect::operator*(const double d) {
  Svect vreturn(*this);
  vreturn *= d;
  return vreturn;
} // end "*" operator definition


/*
** This version of the  "*" operator multiplies two vectors together element-by-element. 
** If the vectors are not of equal size, it returns the vector on the lhs of the "*".
*/
Svect Svect::operator*(const Svect &v) {
  Svect vreturn(*this);
  vreturn *= v;
  return vreturn;
} // end "*" operator definition

/*
** The "+=" operator when used with two vectors adds another vector element-by-element.
** to this one. If the vectors are not of equal size, it does nothing.
*/
Svect& Svect::operator+=(const Svect &v) {
  int    i;
  list<Datapoint>::const_iterator itc;

  if (v.size() == sz) {	
    if (sz < CSZ) { // all values should be cached if this is true
      for (i=0; i<sz; i++) { if (v.cache_index[i] != -1) element_c(i) += *v.cache_dp[i]; }
    } // end if (sz)
    else {
      itc = v.a.begin();
      while (itc != v.a.end()) {
	i = (*itc).i;
	element_c(i) += (*itc).d;
	itc++;
      } // end while (it)
    } // end else (sz)
  } // end if (v)

  return *this;
} // end "+=" operator definition

/*
** The "+" operator adds two vectors together element-by-element. If the vectors are
** not of equal size, it returns the vector on the lhs of the "+".
*/
Svect Svect::operator+(const Svect &v) {
  Svect vreturn(*this);
  vreturn += v;
  return vreturn;
} // end "+" operator defnition

/*
** The "-=" operator when used with two vectors subtracts another vector element-by-element.
** from this one. If the vectors are not of equal size, it does nothing.
*/
Svect& Svect::operator-=(const Svect &v) {
  int i;
  list<Datapoint>::const_iterator itc;

  if (v.size() == sz) {	
    if (sz < CSZ) { // all values should be cached if this is true
      for (i=0; i<sz; i++) { if (v.cache_index[i] != -1) element_c(i) -= *v.cache_dp[i]; }
    } // end if (sz)
    else {
      itc = v.a.begin();
      while (itc != v.a.end()) {
	i = (*itc).i;
	element_c(i) -= (*itc).d;
	itc++;
      } // end while (it)
    } // end else (sz)
  } // end if (v)

  return *this;
} // end "-=" operator definition

/*
** The "-" operator subtracts two vectors element-by-element. If the vectors are
** not of equal size, it returns the vector on the lhs of the "-".
*/
Svect Svect::operator-(const Svect &v) {
  Svect vreturn(*this);
  vreturn -= v;
  return vreturn;
} // end "-" operator definition

/*
** This assignment operator uses the copy() function to copy from one vector to another
** as long as they are the same size.  Otherwise it does nothing.
*/
Svect& Svect::operator=(const Svect &v) { copy(v); return *this; }

/*
** This assignment operator uses the setall() function to copy a double to every element
** in the vector.
*/
Svect& Svect::operator=(const double d) { setall(d); return *this; }

/*
** The bracket ("[]") operator allows accessing an individual element in the vector. The first
** version is the "get" function, and the second version is the "set" function.
*/
double Svect::operator[](int i) const{
  if (i < sz) return element(i); else return element(sz-1);
} // end "[]" (get) operator definition
double& Svect::operator[](int i) {
  if (i < sz) return element_c(i); else return element_c(sz-1);
} // end "[]" (set) operator definition


/*
** The resize() function resizes the vectors and destroys the data (sets to zero).
*/
bool Svect::resize(int n) {
  // if ensure that the list is empty
  a.erase(a.begin(),a.end());
  // set the new size
  sz = n;
  // zero out the cache
  memset(cache_index, -1, CSZ*sizeof(int));

  return true; // this basic case always returns true
} // end resize()

/*
** The copy() function copies the data of one vector to another and returns "true"
** if they are the same size.  Otherwise, it does nothing and returns "false".
*/
bool Svect::copy(const Svect &v) {
  list<Datapoint>::const_iterator it;
  Datapoint dp;

  // resetting this vector size to the new vector size
  resize(v.sz);

  // copying the list data
  it = v.a.begin();
  while (it != v.a.end()) { 
    dp.i = (*it).i; 
    dp.d = (*it).d;     
    a.push_front(dp);
    cache_it[dp.i%CSZ]    = a.begin();
    cache_index[dp.i%CSZ] = dp.i;
    cache_dp[dp.i%CSZ]    = &((*a.begin()).d);
    it++; 
  }

  return true;  

} // end copy()

/*
** The sum() function returns a summation of all elements in a vector.
*/
double Svect::sum(void) {
  list<Datapoint>::iterator it;
  double sum=0.0;

  if (sz < CSZ) { // all values should be cached if this is true
    for (int i=0; i<sz; i++) { if (cache_index[i] != -1) sum+= *cache_dp[i]; }
  } // end if (sz)
  else {
    it = a.begin();
    while (it != a.end()) {
      sum += (*it).d;
      it++;
    } // end while (it)
  } // end else (sz)

  return sum;
} // end sum()

/*
** The exp() function takes the exponential function of every element.  Note that
** zeroes will potentially become explicit elements.
*/
void Svect::exp_elem(void) {
  double d;

  for (int i=0; i<sz; i++) {
    d = exp(element(i));
    if (d > 0.00001) element_c(i) = d; else remove(i);
  } // end for (i)

} // exp_elem()

/*
** The apply_threshold() function sets values greater than or equal to
** the threshold to one and values less than the threshold to zero. The 
** threshold supplied must be greater than zero and less than or equal
** to one.
*/
void Svect::apply_threshold(double f) {
  list<Datapoint>::iterator it;
  double d;

  if ((f > 0.0) && (f <= 1.0)) {

    it = a.begin();
    while (it != a.end()) {
      if ((*it).d >= f) { sete(it, 1.0); it++; }
      else                it = remove(it);
    } // end while (it)

  } // end if (f)

} // end apply_threshold()

/*
******************************************************************************
******************** Dvect CLASS DEFINITION BELOW HERE ***********************
******************************************************************************
*/

/*
** Overloading the "<<" operator allows outputing the elements to an output stream.
*/
ostream& operator<<(ostream& os, const Dvect& v) {
  os << "[ ";
  for (int i=0; i<v.size(); i++) os << v.element(i) << " ";
  os << "]";
  return os;
}

/*
** Overloading the ">>" operator allows inputing the elements from an input stream.
*/
istream& operator>>(istream& is, Dvect& v) {
  for (int i=0; i<v.size(); i++) is >> v[i];
  return is;
}

/*
** The default constructor simply creates an empty vector of zero size.
*/
Dvect::Dvect(void) { 
  a  = nullptr;
  sz = 0;
}

/*
** The alternate constructor creates a vector of size n with all of the elements
** set to zero.
*/
Dvect::Dvect(int n) { 
  // setting "a" to nullptr so that the resize function does not attempt to delete it
  a = nullptr;
  // using the resize function to initialize the vector
  resize(n);
}

/*
** The copy constructor creates a new vector that is exactly like the input vector,
** but occupies distinctly different memory.
*/
Dvect::Dvect(const Dvect &v) {
  // setting "a" to nullptr so that the resize function does not attempt to delete it
  a = nullptr;
  // using the resize function to initialize the vector, if it works copy the input
  if (resize(v.size())) copy(v);
}

/*
** The destructor deallocates any memory that was allocated.
*/
Dvect::~Dvect(void) {
  // deallocating the memory set aside for the vector
  if (a != nullptr) delete a;
}

/*
** The following functions are simple get/set functions.
*/
double Dvect::element(int i) const { return a[i]; }
void   Dvect::setall(double d)     { for (int i=0; i<sz; i++) a[i] = d; }
void   Dvect::set(int i,double d)  { if (i < sz) a[i] = d; }
int    Dvect::size(void) const     { return sz; }

/*
** The "*=" unary operator multiplies another vector with this one element-by-element.  
** This does not correspond to a true matrix multiplication. If the vectors are not of 
** equal size, it does nothing.
*/
Dvect& Dvect::operator*=(const Dvect &v) {
  if (v.size() == sz) {	for (int i=0; i<sz; i++) a[i] *= v.a[i]; }
  return *this;
}

/*
** This version of the "*=" unary operator simply multiplies every element in the
** vector by a constant.
*/
Dvect& Dvect::operator*=(const double d) {
  for (int i=0; i<sz; i++) a[i] *= d;
  return *this;
}

/*
** This version of the "*" operator multiplies a vector by a constant.
*/
Dvect Dvect::operator*(const double d) {
  Dvect vreturn(*this);
  vreturn *= d;
  return vreturn;
}

/*
** This version of the  "*" operator multiplies two vectors together element-by-element. 
** If the vectors are not of equal size, it returns the vector on the lhs of the "*"
** operator.
*/
Dvect Dvect::operator*(const Dvect &v) {
  Dvect vreturn(*this);
  vreturn *= v;
  return vreturn;
}

/*
** The "+=" operator adds another vector with this one element-by-element.
** If the vectors are not of equal size, it does nothing.
*/
Dvect& Dvect::operator+=(const Dvect &v) {
  if (v.size() == sz) {	for (int i=0; i<sz; i++) a[i] += v.a[i]; }
  return *this;
}

/*
** The "+" operator adds two vectors together element-by-element. If the vectors are
** not of equal size, it returns the vector on the lhs of the "+".
*/
Dvect Dvect::operator+(const Dvect &v) {
  Dvect vreturn(*this);
  vreturn += v;
  return vreturn;
}

/*
** The "-=" operator subtracts another vector from this one element-by-element.
** If the vectors are not of equal size, it does nothing.
*/
Dvect& Dvect::operator-=(const Dvect &v) {
  if (v.size() == sz) {	for (int i=0; i<sz; i++) a[i] -= v.a[i]; }
  return *this;
}

/*
** The "-" operator subtracts two vectors element-by-element. If the vectors are
** not of equal size, it returns the vector on the lhs of the "-".
*/
Dvect Dvect::operator-(const Dvect &v) {
  Dvect vreturn(*this);
  vreturn -= v;
  return vreturn;
}

/*
** This assignment operator uses the copy() function to copy from one vector to this one.
** This vector is resized to correspond to the input vector and then the data is copied.
*/
Dvect& Dvect::operator=(const Dvect &v) {
  resize(v.size());
  copy(v);
  return *this;
}

/*
** This assignment operator uses the setall() function to copy a double to every element
** in the vector.
*/
Dvect& Dvect::operator=(const double d) {
  setall(d);
  return *this;
}

/*
** The bracket ("[]") operator allows accessing an individual element in the vector. If
** an index is chosen that is greater than the size of the vector, the last element in
** the vector is returned.  There is no protection for submitting a negative index; it is
** assumed that the user of this class would know that negative numbers are categorically
** illegal in this context.  The first version is the "get" version of the bracket overload,
** while the next version is the "set" version of the bracket overload.
*/
double Dvect::operator[](int i) const{
  if (i < sz) return a[i]; else return a[sz-1];
}
double& Dvect::operator[](int i) {
  if (i < sz) return a[i]; else return a[sz-1];
}


/*
** The resize() function resizes the vectors and destroys any data that might already
** exist (sets all elements to zero).
*/
bool Dvect::resize(int n) {
  // if the array is already allocated, deallocate it
  if (a != nullptr) delete a;
  // allocating a new vector ("a" for array)
  a = nullptr;
  a = new double[n];
  // if the allocation was a success, the size is stored in "size"
  // otherwise, size is set to 0 and failure is returned
  if (a != nullptr) sz = n; else { sz = 0; return false; }
  // initializing the new vector with all zeroes
  for (int i=0; i<n; i++) a[i]=0;
  // return success
  return true;
}

/*
** The copy() function copies the contents of one vector to another and returns "true"
** if they are the same size.  Otherwise, it does nothing and returns "false".
*/
bool Dvect::copy(const Dvect &v) {
  if (v.size() == sz) {	for (int i=0; i<sz; i++) a[i]=v.a[i]; return true;  }
  else                {                                       return false; }
}

/*
** The sum() function returns a summation of all elements in a vector.
*/
double Dvect::sum(void) {
  double sum=0.0;
  for (int i=0; i<sz; i++) sum+=a[i];
  return sum;
}

/*
** The exp() function takes the exponential function of every element.
*/
void Dvect::exp_elem(void) {
  for (int i=0; i<sz; i++) a[i] = exp(a[i]);
}

/*
** The apply_threshold() function sets values greater than or equal to
** the threshold to one and values less than the threshold to zero.
*/
void Dvect::apply_threshold(double d) {
  for (int i=0; i<sz; i++) a[i] = (a[i] >= d)?1.0:0.0;
}
