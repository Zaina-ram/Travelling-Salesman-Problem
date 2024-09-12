#include <vector>
#include <iostream>
#include <tuple>
#include <string>

#ifdef TEST
    #define SEPARATOR " "
#else
    #define SEPARATOR "\n"
#endif
template <class T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
    int n = vec.size();
    std::string tmp = "";
    for(int i = 0; i < n; i++)
    {
       tmp += to_string(vec.at(i));
       tmp += SEPARATOR;
    }
    out << tmp << std::endl;
    return out;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::tuple<T, T> tup)
{
    out << "[" << std::get<0>(tup) << "," << std::get<1>(tup) << "]";
    return out;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::tuple<T, T, T> tup)
{
    out << "[" << std::get<0>(tup) << "," << std::get<1>(tup)  << "," << std::get<2>(tup) << "]";
    return out;
}