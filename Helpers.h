#ifndef HELPERS_H
#define HELPERS_H
#include <tuple>
#include <string>
#include <sstream>
template<typename T>
int ci_find_substr( const T& str1, const T& str2, const std::locale& loc = std::locale() )
{
    T::const_iterator it = std::search( str1.begin(), str1.end(), 
        str2.begin(), str2.end(), my_equal<T::value_type>(loc) );
    if ( it != str1.end() ) return it - str1.begin();
    else return -1; // not found
}

template<typename charT>
struct my_equal {
    my_equal( const std::locale& loc ) : loc_(loc) {}
    bool operator()(charT ch1, charT ch2) {
        return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
    }
private:
    const std::locale& loc_;
};
namespace
{
bool sortbysecond (const std::tuple<std::string,std::string> &lhs, const std::tuple<std::string,std::string> &rhs){
  return std::get<1>(lhs) < std::get<1>(rhs);
}
bool sortbyfirst (const std::tuple<std::string,std::string> &lhs, const std::tuple<std::string,std::string> &rhs){
  return std::get<0>(lhs) < std::get<0>(rhs);
}
std::string single_space(std::string const &input) { 
    std::istringstream buffer(input);
    std::ostringstream result;

    std::copy(std::istream_iterator<std::string>(buffer),
              std::istream_iterator<std::string>(),
              std::ostream_iterator<std::string>(result, " "));
    return result.str();
}
}
#endif