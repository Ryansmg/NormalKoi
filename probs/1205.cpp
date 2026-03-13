/**
 * koiLib by gs24055
 * Version 1.1 (260313)
 */

// 입력 형식이 틀렸을 시 런타임 에러 AssertionFailed
#define INPUT_FORMAT_CHECK true
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
                    std::cout.flush();
                    assert(impl::is_eof());
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

    read_single(int, Int)
    read_single(long long, Long)
    read_single(double, Double)
    read_single(long double, LDouble)
    read_single(std::string, Str)
    read_single(char, Char)

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

int main() {
    using i64 = long long;
    constexpr i64 mod1 = 1'000'000'007;
    vector<i64> dp1(1001000), dp2(1001000), dp3(1001000);
    dp1[1] = 1; dp2[1] = 2; dp3[1] = 2;
    for(i64 i = 2; i <= 1000010; i++) dp3[i] = (dp3[i-1] * 2 + 2) % mod1;
    for(i64 i = 2; i <= 1000010; i++) {
        dp1[i] = (dp2[i-1] + 1 + dp3[i-1]) % mod1;
        dp2[i] = (dp1[i-1] + 2 + 2*dp3[i-1]) % mod1;
    }
    i64 n = readInt(true);
    cout << dp2[n] << '\n';
}
