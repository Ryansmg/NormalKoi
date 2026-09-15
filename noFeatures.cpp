/**
 * koiLib by gs24055
 * only with input functions
 * Compilable in C++11
 * Compatible with v1.43
 */

#pragma region koiLib

#include <iostream>
#include <vector>
#include <array>
#include <tuple>
#include <string>
#include <type_traits>

namespace koi_lib {
    template <typename T> T read() { T x; std::cin >> x; return x; }
    template <typename T> T read(bool) { return read<T>(); }
    template <typename T> T read(char) { return read<T>(); }
    template <typename... Args> int readInt(Args... args) { return read<int>(args...); }
    template <typename... Args> long long readLong(Args... args) { return read<long long>(args...); }
    template <typename... Args> double readDouble(Args... args) { return read<double>(args...); }
    template <typename... Args> long double readLDouble(Args... args) { return read<long double>(args...); }
    template <typename... Args> std::string readStr(Args... args) { return read<std::string>(args...); }
    template <typename... Args> char readChar(Args... args) { return read<char>(args...); }

    template <typename T, std::size_t N>
    std::array<T, N> readArray() {
        std::array<T, N> a{};
        for (std::size_t i = 0; i < N; i++) std::cin >> a[i];
        return a;
    }

    template <std::size_t N> std::array<int, N> readInts() { return readArray<int, N>(); }
    template <std::size_t N> std::array<long long, N> readLongs() { return readArray<long long, N>(); }
    template <std::size_t N> std::array<double, N> readDoubles() { return readArray<double, N>(); }
    template <std::size_t N> std::array<long double, N> readLDoubles() { return readArray<long double, N>(); }
    template <std::size_t N> std::array<std::string, N> readStrs() { return readArray<std::string, N>(); }

    namespace impl {
        template <std::size_t idx, typename... Ts>
        // ReSharper disable once CppUseTypeTraitAlias
        typename std::enable_if<idx == sizeof...(Ts)>::type
        read_tuple(std::tuple<Ts...>&) {}

        template <std::size_t idx, typename... Ts>
        // ReSharper disable once CppUseTypeTraitAlias
        typename std::enable_if<idx < sizeof...(Ts)>::type
        read_tuple(std::tuple<Ts...>& t) {
            std::cin >> std::get<idx>(t);
            read_tuple<idx + 1>(t);
        }
    }

    template <typename... Ts>
    std::tuple<Ts...> read() {
        std::tuple<Ts...> t;
        impl::read_tuple<0>(t);
        return t;
    }

    template <typename T = long long>
    std::vector<T> readArr(int n, char) {
        std::vector<T> v(n);
        for (int i = 0; i < n; i++) std::cin >> v[i];
        return v;
    }

    void get() {}

    template <typename T, typename... Ts>
    void get(T& first, Ts&... rest) {
        std::cin >> first; get(rest...);
    }

    bool readEof() { return std::cin.eof(); }
}

using std::cout;
using namespace koi_lib;

/** koiLib end **/
#pragma endregion

#include <algorithm>
#include <cmath>
#include <stack>
#include <utility>
#include <queue>
#include <set>
#include <map>
#include <numeric>
using namespace std;
using i64 = long long;

i64 dst(const array<i64, 2>& a, const array<i64, 2>& b) {
    return (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]);
}

int main() {
    int n = readInt(true);
    vector<array<i64, 2>> h(n+1);
    for(int i=1; i<=n; i++) h[i][0] = readInt(), h[i][1] = readInt(true);
    int ans = 0;
    vector<int> a(n+1, -214748367);
    a[0] = 0;
    struct edge {
        i64 s, e, d;
    };
    vector<edge> es;
    for(int i=1; i<=n; i++) {
        es.push_back({0, i, dst(h[0],h[i])});
        for(int j=i+1; j<=n; j++) {
            es.push_back({i, j, dst(h[i], h[j])});
            es.push_back({j, i, dst(h[i], h[j])});
        }
    }
    sort(es.begin(), es.end(), [&](const edge& e1, const edge& e2) {
        return e1.d > e2.d;
    });
    i64 prvD = 1557;
    auto cur = a;
    for(const edge& e : es) {
        if(prvD != e.d) prvD = e.d, a = cur;
        ans = max(ans, cur[e.e] = max(cur[e.e], a[e.s] + 1));
    }
    cout << ans;
}
