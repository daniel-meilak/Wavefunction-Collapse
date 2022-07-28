#pragma once

#include<cmath>
#include<compare> // C++20
#include<functional>

//================================================================
// Struct
//================================================================

template< typename Int = int >
struct Point_t{
   Int x,y;

   constexpr Point_t(Int x, Int y): x(x), y(y){};
   constexpr Point_t(const Point_t<Int>& p) = default;
   constexpr Point_t(): x(0), y(0){};

   // spaceship operator defines "==","!=",">","<",">=","<="
   auto operator<=>(const Point_t&) const = default;

   Point_t& operator+=(const Point_t& p);
   Point_t& operator-=(const Point_t& p);
   Point_t& operator*=(const Int& i);
   Point_t& operator/=(const Int& i);
};

using Point = Point_t<int>;
using tileState = Point;

//================================================================
// Member functions
//================================================================

template< typename Int >
Point_t<Int> &Point_t<Int>::operator+=(const Point_t &p){
   this->x += p.x;
   this->y += p.y;
   return *this;
}

template< typename Int >
Point_t<Int> &Point_t<Int>::operator-=(const Point_t &p){
   this->x -= p.x;
   this->y -= p.y;
   return *this;
}

template< typename Int >
Point_t<Int> &Point_t<Int>::operator*=(const Int &i){
   this->x *= i;
   this->y *= i;
   return *this;
}

template< typename Int >
Point_t<Int> &Point_t<Int>::operator/=(const Int &i){
   this->x /= i;
   this->y /= i;
   return *this;
}

//================================================================
// Operators
//================================================================

template< typename Int >
Point_t<Int> operator+(const Point_t<Int> &lhs, const Point_t<Int> &rhs){
   return {lhs.x+rhs.x, lhs.y+rhs.y};
}

template< typename Int >
Point_t<Int> operator-(const Point_t<Int> &lhs, const Point_t<Int> &rhs){
   return {lhs.x-rhs.x, lhs.y-rhs.y};
}

//================================================================
// Hashing function
//================================================================

namespace std {
   template < typename Int > struct hash<Point_t<Int>> {
      typedef Point_t<Int> argument_type;
      typedef std::size_t result_type;
      std::size_t operator()(const Point_t<Int>& p) const noexcept {
         return std::hash<Int>()(p.x ^ (p.y << 4));
      }
   };
}

//================================================================
// Other functions
//================================================================

// manhattan distance between two Point_ts
template< typename Int >
Int manhattan(const Point_t<Int> &p1, const Point_t<Int> &p2){
    return std::abs(p2.x-p1.x) + std::abs(p2.y-p1.y);
}
