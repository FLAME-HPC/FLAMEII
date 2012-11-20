#ifndef MESSAGE_DATATYPES_HPP_
#define MESSAGE_DATATYPES_HPP_

// Datatype of our message
typedef struct {
  double x, y, z;
  int id;
} location_message;

// The structure must support the << operator for it to be store in the board
inline std::ostream &operator<<(std::ostream &os, const location_message& ob) {
  os << "{" << ob.x << ", " << ob.y << ", " << ob.z << ", " << ob.id << "}";
  return os;
}

#endif  // MESSAGE_DATATYPES_HPP_
