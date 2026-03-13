/**
 * koiLib by gs24055
 * Version 1.11 (260313)
 */

// 입력 형식이 틀렸을 시 런타임 에러 AssertionFailed
#define INPUT_FORMAT_CHECK false
/////////////////// 세부 항목 설정
// 입력 파일의 마지막이 '\n'으로 끝나는지 확인
#define CHECK_NON_EOL_EOF true
///////////////////

// 입력 파일을 받아 올바른 형식의 입력 파일을 생성하기
#define MAKE_INPUT_FILE false

// 입력 버퍼 크기. 최대 입력 길이보다 크도록 설정
#define INPUT_BUFFER_SIZE 16777216

#include <bits/stdc++.h>

namespace koi_lib {
    namespace impl {
#if INPUT_FORMAT_CHECK
#define ifc(msg) (void) (koi_lib::impl::kl_wrong_input_format = true, \
    koi_lib::impl::wrong_input_format_reason = msg, \
    (assert(((void) msg, false))))
#else
#define ifc(...)
#endif

        bool kl_wrong_input_format = false;
        const char* wrong_input_format_reason = "null";

        char kl_read_buf[INPUT_BUFFER_SIZE];
        std::vector<std::string_view> kl_tokens;
        std::vector<char> kl_separators;
        int kl_buf_len = 0;
        int kl_to_read_idx = 0;

        void kl_read_init() {
            // int len = read(0, kl_read_buf, INPUT_BUFFER_SIZE - 1);
            assert("input file is larger than INPUT_BUFFER_SIZE, please increase the buffer."
                && kl_buf_len < INPUT_BUFFER_SIZE - 1);
            // && len < INPUT_BUFFER_SIZE - 1);

            if(kl_buf_len == kl_to_read_idx) {
                int c = std::cin.get();
                if(c == EOF) {
#if CHECK_NON_EOL_EOF
                    ifc("unexpected end of file");
#endif
                    c = '\n';
                }
                kl_read_buf[kl_buf_len++] = c;
            }
        }

        bool is_eof() {
            if(kl_buf_len != kl_to_read_idx) return false;
            int c = std::cin.get();
            if(c == EOF) return true;
            assert("input file is larger than INPUT_BUFFER_SIZE, please increase the buffer."
                && kl_buf_len < INPUT_BUFFER_SIZE - 1);
            kl_read_buf[kl_buf_len++] = c;
            return false;
        }

        char peekc() {
            kl_read_init();
            return kl_read_buf[kl_to_read_idx];
        }

        char getc() {
            kl_read_init();
            return kl_read_buf[kl_to_read_idx++];
        }

        bool is_separator(char c, char additional_separator) {
            return c == ' ' || c == '\n' || c == additional_separator;
        }

        std::pair<std::string_view, char> get_token(char expected_end) {
            while(is_separator(peekc(), expected_end)) {
                getc();
                ifc("first character is separator");
            }
            int start = kl_to_read_idx;
            char last = 0;
            while(true) {
                if(is_separator(last = getc(), expected_end))
                    break;
            }
            if(last != expected_end)
                ifc("unexpected end of token");

            int end = kl_to_read_idx - 1;
            if constexpr(MAKE_INPUT_FILE) {
                kl_tokens.emplace_back(kl_read_buf + start, end - start);
                kl_separators.push_back(expected_end);
            }
            return { std::string_view(kl_read_buf + start, end - start), last };
        }

        void read_expected_char(char expected) {
            char c = getc();
            if(c != expected)
                ifc("unexpected char at read_expected_char");

            if constexpr(MAKE_INPUT_FILE) {
                kl_tokens.emplace_back(kl_read_buf + kl_to_read_idx - 1, 1);
                kl_separators.push_back(-1);
            }
        }

        template <typename target>
        target convert_sv(const std::string_view& s) = delete;

#define cvsv_from_chars(type)                                \
    template <>                                              \
    type convert_sv<type>(const std::string_view& s) {       \
        type res;                                            \
        std::from_chars(s.data(), s.data() + s.size(), res); \
        return res;                                          \
    }

        cvsv_from_chars(long long)
        cvsv_from_chars(int)
#ifdef __cpp_lib_to_chars
        // 성능 면에서 나으나 컴파일러 버전에 따라 지원하지 않음
        cvsv_from_chars(double)
        cvsv_from_chars(long double)
#else
        template <>
        double convert_sv<double>(const std::string_view& s) {
            auto tmp = std::string(s);
            return std::strtod(tmp.c_str(), nullptr);
        }

        template <>
        long double convert_sv<long double>(const std::string_view& s) {
            auto tmp = std::string(s);
            return std::strtold(tmp.c_str(), nullptr);
        }
#endif

        template <>
        std::string convert_sv<std::string>(const std::string_view& s) {
            return std::string(s);
        }

        template <>
        char convert_sv<char>(const std::string_view& s) {
            assert(s.size() == 1);
            return s[0];
        }

        template <int arg_cnt = 1>
        std::array<std::string_view, arg_cnt> readTokens() {
            std::array<std::string_view, arg_cnt> res;
            for(int i = 0; i < arg_cnt; i++) {
                auto [token, end] = get_token(i == arg_cnt - 1 ? '\n' : ' ');
                res[i] = token;
            }
            return res;
        }

        std::string_view readToken(char expected_end) {
            auto [token, end] = get_token(expected_end);
            if(end != expected_end)
                ifc("unexpected end of token");

            return token;
        }

        template <typename... Types, size_t... Is>
        std::tuple<Types...> read_tuple_impl(const std::array<std::string_view,
            sizeof...(Types)>& tokens, std::index_sequence<Is...>) {
            return std::make_tuple(
                impl::convert_sv<Types>(tokens[Is])...
            );
        }

        template <typename... Types>
        std::tuple<Types...> read_tuple() {
            constexpr size_t arg_cnt = sizeof...(Types);
            auto tokens = readTokens<arg_cnt>();
            return read_tuple_impl<Types...>(tokens, std::make_index_sequence<arg_cnt>{});
        }

        void print_input_file() {
            assert(kl_tokens.size() == kl_separators.size());
            for(int i = 0; i < static_cast<int>(kl_tokens.size()); i++) {
                std::cout << kl_tokens[i];
                if(kl_separators[i] != -1) std::cout << kl_separators[i];
            }
        }

        struct kl_init {
            kl_init() {
                std::ios::sync_with_stdio(false);
                std::cin.tie(nullptr);
                std::cout.tie(nullptr);
            }

            ~kl_init() {
                if constexpr(INPUT_FORMAT_CHECK) {
                    assert("This should not happen." && !kl_wrong_input_format);
                }

                if constexpr(MAKE_INPUT_FILE) {
                    print_input_file();
                }
            }
        } k_l_i_;

#if MAKE_INPUT_FILE
        struct null_stream {
            template <typename T>
            null_stream& operator<<(const T&) {
                return *this;
            }
            void tie(void*) {}
            void precision(int) {}
            void width(int) {}
            void flush() {}
        } kl_cout_cancel;
#define cout koi_lib::impl::kl_cout_cancel
#endif

    }

#define read_single(type, Type) \
    /**
     * @param readEol false인 경우 공백 문자를, true인 경우 줄바꿈을 받음.
     *                해당하지 않는 문자를 입력받은 경우 잘못된 입력 형식으로 판단.
     */ \
    type read##Type(bool readEol = false) {\
        auto token = impl::readToken(readEol ? '\n' : ' ');\
        return impl::convert_sv<type>(token);\
    }

#define read_single_with_end(type, Type) \
    type read##Type(char end = ' ') {\
        auto token = impl::readToken(end);\
        return impl::convert_sv<type>(token);\
    }

#define read_single_def(type, Type) read_single(type, Type) read_single_with_end(type, Type)

    read_single_def(int, Int)
    read_single_def(long long, Long)
    read_single_def(double, Double)
    read_single_def(long double, LDouble)
    read_single_def(std::string, Str)
    read_single_def(char, Char)

#define read_multiple(type, Type) \
    template <int arg_cnt = 1>\
    std::array<type, arg_cnt> read##Type##s() {\
        std::array<type, arg_cnt> res;\
        auto tokens = impl::readTokens<arg_cnt>();\
        for(int i = 0; i < arg_cnt; i++)\
            res[i] = impl::convert_sv<type>(tokens[i]);\
        return res;\
    }

    read_multiple(int, Int)
    read_multiple(long long, Long)
    read_multiple(double, Double)
    read_multiple(long double, LDouble)
    read_multiple(std::string, Str)

    template <typename Type = long long>
    Type read(bool readEol = false) {
        return impl::convert_sv<Type>(
            impl::readToken(readEol ? '\n' : ' ')
            );
    }

    template <typename Type, typename Type2, typename... Types>
    std::tuple<Type, Type2, Types...> read() {
        return impl::read_tuple<Type, Type2, Types...>();
    }

    template <typename T = long long>
    std::vector<T> readArr(int n) {
        std::vector<T> arr(n);
        for(int i = 0; i < n; i++)
            arr[i] = impl::convert_sv<T>(impl::readToken(i == n - 1 ? '\n' : ' '));
        return arr;
    }

    template <typename... Args>
    // ReSharper disable once CppDFAUnreadVariable
    void get(Args&... args) {
        constexpr size_t arg_cnt = sizeof...(Args);
        auto tokens = impl::readTokens<arg_cnt>();
        int i = 0;
        // ReSharper disable once CppDFAUnusedValue
        ((args = impl::convert_sv<Args>(tokens[i++])), ...);
    }

    std::string readToken(char expected_end) {
        return std::string(impl::readToken(expected_end));
    }

    void readEof() {
#if INPUT_FORMAT_CHECK
#ifndef LOCAL
        assert(impl::is_eof());
#endif
#endif
    }

#define scanf "Cannot use C-style input method"
#define printf "Cannot use C-style output method"
#define cin "Cannot use C++-style input method"

#if MAKE_INPUT_FILE
#define USE_COUT
#else
#define USE_COUT using std::cout
#endif

#if INPUT_FORMAT_CHECK
#define ensure(...) assert(__VA_ARGS__)
#else
#define ensure(...)
#endif

#undef ifc
#undef cvsv_from_chars
#undef read_single
#undef read_multiple
}

using namespace koi_lib;
using namespace std;

constexpr long long mod = 100'000'007;
struct mint {
    // if mod is not prime
    // static long long modInv(long long a) {
    //     long long b = mod, u = 1, v = 0;
    //     while (b) {
    //         long long t = a / b; a -= t * b;
    //         std::swap(a, b); u -= t * v; std::swap(u, v);
    //     }
    //     u %= mod; if (u < 0) {u += mod;} return u;
    // }

    static inline long long pow_(long long a, long long b) {
        a %= mod; long long ans=1;
        while(b){if(b&1)ans=ans*a%mod;b>>=1;a=a*a%mod;}
        return ans;
    }

    static long long modInv(long long a) { return pow_(a, mod - 2); }


    long long v = 0;

    mint() = default;
    // ReSharper disable once CppNonExplicitConvertingConstructor
    constexpr mint(long long val) : v(val % mod) { if(v < 0) v += mod; } // NOLINT(*-explicit-constructor)
    mint& operator=(const mint& b) = default;

    mint operator-() const { return { -v }; }
    mint inv() const { return {modInv(v)}; }

    mint& operator++() { if(++v >= mod) { v -= mod; } return *this; }
    mint operator++(signed) { mint ret = *this; if(++v >= mod) { v -= mod; } return ret; }
    mint& operator--() { if(--v < 0) { v += mod; } return *this; }
    mint operator--(signed) { mint ret = *this; if(--v < 0) { v += mod; } return ret; }
    mint operator+(const mint& b) const { return mint(*this) += b; }
    mint operator-(const mint& b) const { return mint(*this) -= b; }
    mint operator*(const mint& b) const { return { v * b.v }; }
    mint operator/(const mint& b) const { return { v * modInv(b.v) }; }
    bool operator==(const mint& b) const { return v == b.v; }

    mint& operator+=(const mint& b) { if((v += b.v) >= mod) { v -= mod; } return *this; }
    mint& operator+=(const mint&& b) { if((v += b.v) >= mod) { v -= mod; } return *this; }
    mint& operator-=(const mint& b) { if((v -= b.v) < 0) { v += mod; } return *this; }
    mint& operator-=(const mint&& b) { if((v -= b.v) < 0) { v += mod; } return *this; }
    mint& operator*=(const mint& b) { v = v * b.v % mod; return *this; }
    mint& operator*=(const mint&& b) { v = v * b.v % mod; return *this; }
    mint& operator/=(const mint& b) { v = v * modInv(b.v) % mod; return *this; }
    mint& operator/=(const mint&& b) { v = v * modInv(b.v) % mod; return *this; }

    friend mint operator+(long long a, const mint& b) { return {b.v + a}; }
    friend mint operator-(long long a, const mint& b) { return {b.v - a}; }
    friend mint operator*(long long a, const mint& b) { return {b.v * a}; }
    friend mint operator/(long long a, const mint& b) { return {modInv(b.v) * a}; }

    friend std::istream& operator>>(std::istream& in, mint& t) {
        in >> t.v; t.v = t.v % mod;
        if(t.v < 0) t.v += mod;
        return in;
    }
    friend std::ostream& operator<<(std::ostream& out, const mint& t) { out << t.v; return out; }
};

#define forn(i, n) for(int i=0; i<n; i++)
template <typename T>
struct Matrix {
    signed r = 0, c = 0;
    std::vector<std::vector<T>> m;
    Matrix() = default;
    Matrix(signed row, signed column, T value) : r(row), c(column), m(row, std::vector<T>(column, value)) {}
    explicit Matrix(std::vector<std::vector<T>> arr) : r(arr.size()), c(arr.empty() ? 0 : arr[0].size()), m(std::move(arr)) {}
    inline auto& operator[](signed index) { return m[index]; }
    inline const auto& operator[](signed index) const { return m[index]; }

    Matrix& operator+=(const Matrix& other) {
        assert(r == other.r && c == other.c);
        for(signed i = 0; i < r; i++)
            for(signed j = 0; j < c; j++)
                m[r][c] += other[r][c];
        return *this;
    }

    Matrix operator+(const Matrix& other) const {
        Matrix ret = *this;
        ret += other;
        return ret;
    }

    Matrix& operator-=(const Matrix& other) {
        assert(r == other.r && c == other.c);
        for(signed i = 0; i < r; i++)
            for(signed j = 0; j < c; j++)
                m[r][c] -= other[r][c];
        return *this;
    }

    Matrix operator-(const Matrix& other) const {
        Matrix ret = *this;
        ret -= other;
        return ret;
    }

    Matrix operator-() const {
        Matrix ret = *this;
        for(signed i = 0; i < r; i++)
            for(signed j = 0; j < c; j++)
                ret.m[r][c] = -ret.m[r][c];
        return ret;
    }

    Matrix operator*(const Matrix& other) const {
        assert(c == other.r);
        Matrix ret(r, other.c, T());
        for(signed lr = 0; lr < r; lr++)
            // ReSharper disable once CppDFANotInitializedField
            for(signed rc = 0; rc < other.c; rc++)
                for(signed i = 0; i < c; i++)
                    ret[lr][rc] += m[lr][i] * other[i][rc];
        return ret;
    }
    Matrix& operator*=(const Matrix& other) { return *this = *this * other; }

    Matrix& operator*=(const T& val) {
        for(signed i = 0; i < r; i++)
            for(signed j = 0; j < c; j++)
                m[i][j] *= val;
        return *this;
    }

    Matrix operator*(const T& val) const {
        Matrix ret = *this; ret *= val; return ret;
    }

    Matrix& naive_mul(const Matrix& other) {
        assert(r == other.r && c == other.c);
        for(signed i = 0; i < r; i++)
            for(signed j = 0; j < c; j++)
                m[i][j] *= other.m[i][j];
        return *this;
    }

    friend Matrix naive_mul(const Matrix& a, const Matrix& b) {
        auto ret = a;
        ret *= b;
        return ret;
    }

    Matrix& operator/=(const T& val) {
        for(signed i = 0; i < r; i++)
            for(signed j = 0; j < c; j++)
                m[i][j] /= val;
        return *this;
    }

    Matrix operator/(const T& val) const {
        Matrix ret = *this; ret /= val; return ret;
    }


    Matrix& operator/=(const Matrix& other) {
        assert(r == other.r && c == other.c);
        for(signed i = 0; i < r; i++)
            for(signed j = 0; j < c; j++)
                m[r][c] /= other[r][c];
        return *this;
    }

    Matrix operator/(const Matrix& other) const {
        Matrix ret = *this;
        ret /= other;
        return ret;
    }

    Matrix& operator%=(const T& val) {
        for(signed i = 0; i < r; i++)
            for(signed j = 0; j < c; j++)
                m[i][j] %= val;
        return *this;
    }

    Matrix operator%(const T& val) const {
        Matrix ret = *this; ret %= val; return ret;
    }

    friend istream& operator>>(istream& in, Matrix& mat) {
        for(signed i = 0; i < mat.r; i++)
            for(signed j = 0; j < mat.c; j++)
                in >> mat.m[i][j];
        return in;
    }

    friend ostream& operator<<(ostream& out, const Matrix& mat) {
        for(signed i = 0; i < mat.r; i++) {
            for(signed j = 0; j < mat.c; j++)
                out << mat.m[i][j] << ' ';
            if(i < mat.r - 1) out << '\n';
        }
        return out;
    }

    struct Row {
        std::vector<std::vector<T>>* p;
        int index;
        auto begin() { return (*p)[index].begin(); }
        auto end() { return (*p)[index].end(); }
        void operator+=(const T& val) { for(T& i : (*p)[index]) i += val; }
        void operator-=(const T& val) { for(T& i : (*p)[index]) i -= val; }
        void operator*=(const T& val) { for(T& i : (*p)[index]) i *= val; }
        void operator/=(const T& val) {
            for(T& i : (*p)[index]) i /= val;
        }
        void operator%=(const T& val) { for(T& i : (*p)[index]) i %= val; }

        void operator+=(const Row& R) {
            for(int i = 0; i < int((*p)[0].size()); i++)
                (*p)[index][i] += (*R->p)[R.index][i];
        }

        void operator-=(const Row& R) {
            for(int i = 0; i < int((*p)[0].size()); i++)
                (*p)[index][i] -= (*R->p)[R.index][i];
        }
        std::vector<T>& operator*() { return (*p)[index]; }
    };

    struct Col {
        std::vector<std::vector<T>>* p;
        int index;
        void operator+=(const T& val) { for(int i = 0; i < p->size(); i++) (*p)[i][index] += val; }
        void operator-=(const T& val) { for(int i = 0; i < p->size(); i++) (*p)[i][index] -= val; }
        void operator*=(const T& val) { for(int i = 0; i < p->size(); i++) (*p)[i][index] *= val; }
        void operator/=(const T& val) { for(int i = 0; i < p->size(); i++) (*p)[i][index] /= val; }
        void operator%=(const T& val) { for(int i = 0; i < p->size(); i++) (*p)[i][index] %= val; }
        struct iter {
            std::vector<std::vector<T>>* p;
            int R, C;
            T& operator*() { return p[R][C]; }
            const T& operator*() const { return p[R][C]; }
            iter operator++() const { return {p, R+1, C}; }
            iter operator--() const { return {p, R-1, C}; }
            iter& operator++(int) { return *this = operator++(); }
            iter& operator--(int) { return *this = operator--(); }
            bool operator==(const iter& b) const { return R == b.R; }
        };
        iter begin() { return {p, 0, index}; }
        iter end() { return {p, p->size(), index}; }

        struct col_vec_ref {
            Col* p;
            friend void swap(col_vec_ref a, col_vec_ref b) {
                int ai = a.p->index, bi = b.p->index;
                for(int i = 0; i < int(a.p->p->size()); i++) {
                    std::swap((*a.p->p)[i][ai], (*a.p->p)[i][bi]);
                }
            }
        };
        col_vec_ref operator*() { return {this}; }
    };

    template <typename T2>
    Row row(T2 index) { return Row(&m, (long long) index); }

    template <typename T2>
    Col col(T2 index) { return Col(&m, (long long) index); }

    Matrix RREF() const {
        Matrix ret = *this;
        int cnt = 0;
        for(int i = 0; i < c && cnt < r; i++) {
            int select = -1;
            for(int j = cnt; j < r; j++) {
                if(ret[j][i] != T(0)) {
                    select = j;
                    break;
                }
            }
            if(select == -1) continue;
            std::swap(ret[cnt], ret[select]);
            T temp = ret[cnt][i];
            ret.row(cnt) /= temp; ret[cnt][i] = T(1);
            for(int j = 0; j < r; j++) {
                if(j == cnt) continue;
                T t = ret[j][i];
                if(t == T(0)) continue;
                for(int k = i; k < c; k++) {
                    ret[j][k] -= t * ret[cnt][k];
                }
                ret[j][i] = T(0);
            }
            cnt++;
        }
        return ret;
    }

    struct Block {
        Matrix* p = nullptr;
        int sr = 0, er = 0, sc = 0, ec = 0;
        static Block whole(Matrix* ptr) { return {ptr, 0, ptr->r - 1, 0, ptr->c - 1}; }
    };

    T cofactor(int row, int col) const {
        vector<vector<T>> nxt;
        forn(i, r) {
            if(i == row) continue;
            nxt.emplace_back();
            forn(j, c) {
                if(j == col) continue;
                nxt.back().push_back(m[i][j]);
            }
        }
        if((row + col) % 2) return Matrix(nxt).determinant() * T(-1);
        return Matrix(nxt).determinant();
    }

    T determinant() const {
        assert(r == c);
        if(r == 1) return m[0][0];
        T res = T(0);
        forn(i, c) res += cofactor(0, i) * m[0][i];
        return res;
    }

    Matrix transpose() const {
        vector<vector<T>> nxt(c, vector<T>(r));
        forn(i, r) forn(j, c) nxt[j][i] = m[i][j];
        return Matrix(nxt);
    }

    Matrix adjoint() const {
        assert(r == c);
        vector<vector<T>> res(r, vector<T>(r));
        forn(i, r) forn(j, c) res[i][j] = cofactor(i, j);
        return Matrix(res).transpose();
    }

    Matrix inverse() const {
        return inverse_with_existence().first;
    }

    pair<Matrix, bool> inverse_with_existence() const {
        assert(r == c);
        vector<vector<T>> nxt(r, vector<T>(r * 2));
        forn(i, r) forn(j, c) nxt[i][j] = m[i][j];
        forn(i, r) nxt[i][i + c] = T(1);
        Matrix R = Matrix(nxt).RREF();
        forn(i, r) forn(j, c) {
            if(i == j && R[i][j] != T(1)) return std::make_pair(Matrix(), false);
            if(i != j && R[i][j] != T(0)) return std::make_pair(Matrix(), false);
        }
        vector<vector<T>> res(r, vector<T>(r));
        forn(i, r) forn(j, c) res[i][j] = R[i][j + c];
        return std::make_pair(Matrix(res), true);
    }
};

template <typename T> inline T pow_(T a, long long b) {
    assert(b > 0);
    T ans = a; b--;
    while(b) {
        if(b&1)ans=ans*a;b>>=1;a=a*a;
    } return ans;
}

// a[i] = a[i-1] + a[i-3]

int main() {
    auto n = readLong('\n');
    Matrix<mint> init(vector<vector<mint>>{
        {1, 1, 2}
    });
    Matrix<mint> mul(vector<vector<mint>>{
        {0, 0, 1},
        {1, 0, 0},
        {0, 1, 1}
    });
    if(n == 1) return cout << 1, 0;
    auto t = pow_(mul, n - 1);
    cout << (init * t)[0][0] << '\n';
}
