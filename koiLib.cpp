/**
 * koiLib by gs24055
 * Version 1.43 (260409)
 */

#define VALIDATOR true
#define FORMATTER false

/////////////////// 세부 항목 설정
// 토큰의 구분자가 예상과 같은지 확인 ("10\n" <-> "10 ")
#define CHECK_TOKEN_END true
// `\n` 대신 `\r\n`이 사용되었는지 확인
#define CHECK_WIN_LINEBREAK true
// 줄의 끝이 " \n"으로 끝나는지 확인
#define CHECK_TRAILING_SPACE true
///////////////////

#pragma region koiLib
// 입력 버퍼 크기. 최대 입력 길이보다 크도록 설정
#define INPUT_BUFFER_SIZE 16777216

/////////////////// 추가 세부 항목 설정
// 입력 파일의 마지막이 '\n'으로 끝나는지 확인
#define CHECK_NON_EOL_EOF false
// 예상된 eof 지점 뒤에 입력이 더 있는지 확인
#define CHECK_TRAILING_INPUT true
// "1.05"를 정수로 변환하는 등 잘못된 변환을 확인
#define CHECK_WRONG_CONVERSION true
///////////////////

#include <array>
#include <cassert>
#include <charconv>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#else
#include <poll.h>
#include <unistd.h>
#endif

namespace koi_lib {
    namespace impl {
#if FORMATTER
#undef VALIDATOR
#define VALIDATOR false
#endif

#if !VALIDATOR
#undef CHECK_NON_EOL_EOF
#undef CHECK_TOKEN_END
#undef CHECK_TRAILING_INPUT
#undef CHECK_TRAILING_SPACE
#undef CHECK_WRONG_CONVERSION
#undef CHECK_WIN_LINEBREAK
#define CHECK_NON_EOL_EOF false
#define CHECK_TOKEN_END false
#define CHECK_TRAILING_INPUT false
#define CHECK_TRAILING_SPACE false
#define CHECK_WRONG_CONVERSION false
#define CHECK_WIN_LINEBREAK false
#endif

        char kl_read_buf[INPUT_BUFFER_SIZE];
        std::vector<std::string_view> kl_tokens;
        std::vector<char> kl_separators;
        int kl_buf_len = 0;
        int kl_to_read_idx = 0;

        // O(kl_buf_len)
        int kl_line_cnt() {
            int c = 1;
            for(int i = 0; i < kl_buf_len; i++) c += (kl_read_buf[i] == '\n');
            return c - (kl_read_buf[kl_buf_len - 1] == '\n');
        }

        void kl_read_init() {
            // int len = read(0, kl_read_buf, INPUT_BUFFER_SIZE - 1);
            assert("input file is larger than INPUT_BUFFER_SIZE, please increase the buffer."
                && kl_buf_len < INPUT_BUFFER_SIZE - 1);
            // && len < INPUT_BUFFER_SIZE - 1);

            if(kl_buf_len == kl_to_read_idx) {
                int c = std::cin.get();
                if(c == EOF) {
                    if constexpr(CHECK_NON_EOL_EOF) {
                        std::cerr << "unexpected end of file at line " << kl_line_cnt() << std::endl;
                        std::exit(1);
                    }
                    c = '\n';
                }
                kl_read_buf[kl_buf_len++] = c;
            }
        }

        bool is_eof() {
            if(kl_to_read_idx != kl_buf_len) return false;
#ifdef _WIN32
            DWORD bytes;
            if(!PeekNamedPipe(GetStdHandle(STD_INPUT_HANDLE), nullptr, 0, nullptr,
                &bytes, nullptr))
                return true;
            return bytes == 0;
#else
            int c = std::cin.get();
            if(c == EOF) return true;
            assert("input file is larger than INPUT_BUFFER_SIZE, please increase the buffer."
                && kl_buf_len < INPUT_BUFFER_SIZE - 1);
            kl_read_buf[kl_buf_len++] = c;
            return false;
#endif
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
            return c == ' ' || c == '\n' || c == '\r' || c == additional_separator;
        }

        std::string escaped_string(char c) {
            if(c == '\n') return "\\n";
            if(c == '\r') return "\\r";
            return {c};
        }

        std::string escaped_string(const std::string& s) {
            std::string res;
            for(char c : s) res.append(escaped_string(c));
            return res;
        }

        std::string escaped_string(const std::string_view& s) {
            std::string res;
            for(char c : s) res.append(escaped_string(c));
            return res;
        }

        std::string_view get_token(char expected_end) {
            while(is_separator(peekc(), expected_end)) {
                getc();
                if constexpr(VALIDATOR) {
                    std::cerr << "get_token: first character is separator" << std::endl;
                    std::exit(1);
                }
            }
            int start = kl_to_read_idx;
            char last = 0;
            while(true) {
                if(is_separator(last = getc(), expected_end))
                    break;
            }

            int end = kl_to_read_idx - 1;

            if(expected_end == '\n' && last == ' ') {
                if constexpr(CHECK_TRAILING_SPACE) {
                    std::cerr << "[TRAILING_SPACE] expected: '\\n', input: ' '" << std::endl;
                    std::exit(1);
                }
                else {
                    while(last == ' ' && is_separator(peekc(), '\n'))
                        last = getc();
                }
            }

            if(expected_end == '\n' && last == '\r') {
                if constexpr(CHECK_WIN_LINEBREAK) {
                    std::cerr << "[WIN_LINEBREAK] expected: '\\n', input: '\\r'" << std::endl;
                    std::exit(1);
                } else {
                    while(last == '\r')
                        last = getc();
                }
            }

            if constexpr(CHECK_TOKEN_END)
                if(last != expected_end) {
                    std::cerr << "unexpected end of token at line " << kl_line_cnt() << std::endl;
                    std::cerr << "expected: '" << escaped_string(expected_end)
                        << "', input: '" << escaped_string(last) << "'" << std::endl;
                    std::exit(1);
                }

            if constexpr(FORMATTER) {
                kl_tokens.emplace_back(kl_read_buf + start, end - start);
                kl_separators.push_back(expected_end);
            }
            return std::string_view(kl_read_buf + start, end - start); // NOLINT(*-return-braced-init-list)
        }

        void read_expected_char(char expected) {
            char c = getc();
            if(c != expected) {
                if constexpr(VALIDATOR) {
                    std::cerr << "unexpected char at read_expected_char" << std::endl;
                    std::cerr << "expected: '" << escaped_string(expected) << "', input: '" <<
                        escaped_string(c) << "'" << std::endl;
                    std::exit(1);
                }
            }

            if constexpr(FORMATTER) {
                kl_tokens.emplace_back(kl_read_buf + kl_to_read_idx - 1, 1);
                kl_separators.push_back(-1);
            }
        }

        template <typename target>
        target convert_sv(const std::string_view& s) = delete;

#define cvsv_from_chars(type)                                                 \
    template <>                                                               \
    type convert_sv<type>(const std::string_view& s) {                        \
        type res;                                                             \
        auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), res); \
        if constexpr(CHECK_WRONG_CONVERSION)                                               \
            if(ec != std::errc() || ptr != s.data() + s.size()) {                          \
                std::cerr << "wrong conversion at convert_sv, '" << escaped_string(s) << "' to " << #type << std::endl;  \
                std::exit(1);                                                 \
            }                                                                 \
        return res;                                                           \
    }

        cvsv_from_chars(long long)
        cvsv_from_chars(int)
#ifdef __cpp_lib_to_chars
        // 성능 면에서 나으나 컴파일러 버전에 따라 지원하지 않음
        cvsv_from_chars(float)
        cvsv_from_chars(double)
        cvsv_from_chars(long double)
#else
        template <>
        double convert_sv<double>(const std::string_view& s) {
            auto tmp = std::string(s);
            char* endPtr;
            double res = std::strtod(tmp.c_str(), &endPtr);
            if constexpr(CHECK_WRONG_CONVERSION)
                if(endPtr != s.data() + s.size()) {
                    std::cerr << "wrong conversion at convert_sv, '" << s << "' to double" << std::endl;
                    std::exit(1);
                }
            return res;
        }

        template <>
        long double convert_sv<long double>(const std::string_view& s) {
            auto tmp = std::string(s);
            char* endPtr;
            long double res = std::strtold(tmp.c_str(), &endPtr);
            if constexpr(CHECK_WRONG_CONVERSION)
                if(endPtr != s.data() + s.size()) {
                    std::cerr << "wrong conversion at convert_sv, '" << s << "' to long double" << std::endl;
                    std::exit(1);
                }
            return res;
        }
#endif

        template <>
        std::string convert_sv<std::string>(const std::string_view& s) {
            return std::string(s);
        }

        template <>
        char convert_sv<char>(const std::string_view& s) {
            if constexpr(CHECK_WRONG_CONVERSION) {
                if(s.size() != 1) {
                    std::cerr << "wrong conversion at convert_sv, '" << s << "' to char" << std::endl;
                    std::exit(1);
                }
            }
            return s[0];
        }

        template <int arg_cnt = 1>
        std::array<std::string_view, arg_cnt> readTokens() {
            std::array<std::string_view, arg_cnt> res;
            for(int i = 0; i < arg_cnt; i++)
                res[i] = get_token(i == arg_cnt - 1 ? '\n' : ' ');
            return res;
        }

        std::string_view readToken(char expected_end) {
            return get_token(expected_end);
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

        bool eof_explicitly_checked = false;

        struct kl_init {
            kl_init() {
                std::ios::sync_with_stdio(false);
                std::cin.tie(nullptr);
                std::cout.tie(nullptr);
            }

            ~kl_init() {
                if constexpr(VALIDATOR) {
                    if constexpr(CHECK_TRAILING_INPUT)
                        if(!is_eof()) {
                            std::cerr << "Trailing input exists." << std::endl;
                            std::exit(1);
                        }
                }

                if constexpr(FORMATTER) {
                    print_input_file();
                }
            }
        } k_l_i_;

        void init_kl_read_buf_() {
            int i = kl_to_read_idx;
            while(!is_eof()) getc();
            kl_to_read_idx = i;
        }

        void write_kl_read_buf_(std::ostream& out = std::cout) {
            for(int i = 0; i < kl_buf_len; i++) out << kl_read_buf[i];
        }

        void kl_ib_wr_e1_() {
            init_kl_read_buf_();
            write_kl_read_buf_(std::cerr);
            std::exit(1);
        }

#if FORMATTER
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
    type read##Type(char end) {\
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

    read_multiple(int, Int) // NOLINT(*-pro-type-member-init)
    read_multiple(long long, Long) // NOLINT(*-pro-type-member-init)
    read_multiple(double, Double) // NOLINT(*-pro-type-member-init)
    read_multiple(long double, LDouble) // NOLINT(*-pro-type-member-init)
    read_multiple(std::string, Str) // NOLINT(*-pro-type-member-init)

    template <typename Type = long long>
    Type read(bool readEol = false) {
        return impl::convert_sv<Type>(
            impl::readToken(readEol ? '\n' : ' ')
            );
    }

    template <typename Type = long long>
    Type read(char end) {
        return impl::convert_sv<Type>(
            impl::readToken(end)
            );
    }

    template <typename Type, typename Type2, typename... Types>
    std::tuple<Type, Type2, Types...> read() {
        return impl::read_tuple<Type, Type2, Types...>();
    }

    template <typename T = long long>
    std::vector<T> readArr(int n, char sep = ' ') {
        std::vector<T> arr(n);
        for(int i = 0; i < n; i++)
            arr[i] = impl::convert_sv<T>(impl::readToken(i == n - 1 ? '\n' : sep));
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

    void readEof() {
        if(impl::eof_explicitly_checked)
            return;
        impl::eof_explicitly_checked = true;
#if VALIDATOR && CHECK_TRAILING_INPUT
        assert(impl::is_eof());
#if FORMATTER
        std::exit(0);  // 정답 계산 등을 생략
#endif
#endif
    }

#define scanf "Cannot use C-style input method"
#define printf "Cannot use C-style output method"
#define cin "Cannot use C++-style input method"

#if FORMATTER
#define USE_COUT
#else
#define USE_COUT using std::cout
#endif

#if VALIDATOR
#define ensure(...) assert(__VA_ARGS__)
#else
#define ensure(...)
#endif

#undef cvsv_from_chars
#undef read_single
#undef read_multiple
}

USE_COUT;
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

int main() {

}
