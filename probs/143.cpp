/**
 * koiLib by gs24055
 * Version 1.3 indev (260402)
 */

#include <bits/stdc++.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <poll.h>
#include <unistd.h>
#endif

// 입력 형식이 틀렸을 시 런타임 에러 AssertionFailed
#define INPUT_FORMAT_CHECK true
/////////////////// 세부 항목 설정
// 입력 파일의 마지막이 '\n'으로 끝나는지 확인
#define CHECK_NON_EOL_EOF false
// 토큰의 구분자가 예상과 같은지 확인 ("10\n" <-> "10 ")
#define CHECK_TOKEN_END true
// 예상된 eof 지점 뒤에 입력이 더 있는지 확인
#define CHECK_TRAILING_INPUT true
// 줄의 끝이 " \n"으로 끝나는 것을 허용
#define ALLOW_TRAILING_SPACE false
///////////////////

// 입력 파일을 받아 올바른 형식의 입력 파일을 생성하기
#define MAKE_INPUT_FILE false

// 입력 버퍼 크기. 최대 입력 길이보다 크도록 설정
#define INPUT_BUFFER_SIZE 16777216

namespace koi_lib {
    namespace impl {
#if MAKE_INPUT_FILE
#undef INPUT_FORMAT_CHECK
#define INPUT_FORMAT_CHECK false
#endif

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
                        ifc("unexpected end of file");
                    }
                    c = '\n';
                }
                kl_read_buf[kl_buf_len++] = c;
            }
        }

        bool is_eof() {
#ifdef _WIN32
            DWORD bytes;
            if(!PeekNamedPipe(GetStdHandle(STD_INPUT_HANDLE), nullptr, 0, nullptr,
                &bytes, nullptr))
                return true;
            return bytes == 0;
#else
            pollfd pfd{};
            pfd.fd = STDIN_FILENO;
            pfd.events = POLLIN;

            int r = poll(&pfd, 1, 0);
            if(r <= 0) return true;

            if(pfd.revents & POLLHUP) return true;

            if(pfd.revents & POLLIN) {
                char c;
                ssize_t n = read(STDIN_FILENO, &c, 1);
                if(n == 0) return true;   // EOF
                if(n > 0) {
                    ungetc(c, stdin);     // 문자 복원
                    return false;
                }
            }

            return true;
#endif
            // int c = std::cin.get();
            // if(c == EOF) return true;
            // assert("input file is larger than INPUT_BUFFER_SIZE, please increase the buffer."
            //     && kl_buf_len < INPUT_BUFFER_SIZE - 1);
            // kl_read_buf[kl_buf_len++] = c;
            // return false;
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

        std::string_view get_token(char expected_end) {
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
            if(expected_end == '\n' && last == ' ') {
                if constexpr(ALLOW_TRAILING_SPACE) {
                    last = getc();
                }
            }

            if constexpr(CHECK_TOKEN_END)
                if(last != expected_end) {
                    std::string ch = (last == '\n' ? "\\n" : std::string(1, last));
                    std::string ex = (expected_end == '\n' ? "\\n" : std::string(1, expected_end));
                    std::cerr << "unexpected end of token at line " << kl_line_cnt() << std::endl;
                    std::cerr << "expected: '" << ex << "', input: '" << ch << "'" << std::endl;
                    ifc("unexpected end of token");
                }

            int end = kl_to_read_idx - 1;
            if constexpr(MAKE_INPUT_FILE) {
                kl_tokens.emplace_back(kl_read_buf + start, end - start);
                kl_separators.push_back(expected_end);
            }
            return std::string_view(kl_read_buf + start, end - start); // NOLINT(*-return-braced-init-list)
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
                if constexpr(INPUT_FORMAT_CHECK) {
                    if constexpr(CHECK_TRAILING_INPUT)
                        assert("Trailing input exists." && is_eof());
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
#if INPUT_FORMAT_CHECK && CHECK_TRAILING_INPUT
        assert(impl::is_eof());
#if MAKE_INPUT_FILE
        std::exit(0);  // 정답 계산 등을 생략
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

USE_COUT;
using namespace koi_lib;

// using namespace std;
using std::less, std::vector;


template <typename T, typename Compare>
inline void merge(const T* s1, const T* e1, const T* s2, const T* e2, T* out, Compare cmp = less<T>()) {
    while(s1 != e1 && s2 != e2) {
        if(cmp(*s1, *s2)) *(out++) = *(s1++);
        else *(out++) = *(s2++);
    }
    while(s1 != e1) *(out++) = *(s1++);
    while(s2 != e2) *(out++) = *(s2++);
}

template <typename T>
inline void merge(const T* s1, const T* e1, const T* s2, const T* e2, T* out) {
    while(s1 != e1 && s2 != e2) {
        if(*s1 <= *s2) *(out++) = *(s1++);
        else *(out++) = *(s2++);
    }
    while(s1 != e1) *(out++) = *(s1++);
    while(s2 != e2) *(out++) = *(s2++);
}

template <typename T, typename Compare>
inline void merge(const vector<T>& a, const vector<T>& b, vector<T>& out, Compare cmp = less<T>()) {
    merge(a.begin(), a.end(), b.begin(), b.end(), out.begin(), cmp);
}

template <typename T>
inline void merge(const vector<T>& a, const vector<T>& b, vector<T>& out) {
    merge(a.begin(), a.end(), b.begin(), b.end(), out.begin());
}

template <typename T, typename Compare>
inline void naive_sort(T* s, T* e, Compare cmp = less<T>()) {
    for(T* i = s; i != e; i++) {
        T* sel = i;
        for(T* j = i + 1; j != e; j++)
            if(!cmp(*sel, *j)) sel = j;
        if(sel != i) swap(*i, *sel);
    }
}

template <typename T>
inline void naive_sort(T* s, T* e) {
    for(T* i = s; i != e; i++) {
        T* sel = i;
        for(T* j = i + 1; j != e; j++)
            if(*sel > *j) sel = j;
        if(sel != i) swap(*i, *sel);
    }
}

template <typename T, typename Compare>
void sort_(T* s, T* e, T* buf, Compare cmp) {
    long long dist = e - s;
    if(dist <= 16) {
        naive_sort(s, e, cmp);
        return;
    }
    sort_(s, s + dist / 2 + 1, buf, cmp);
    sort_(s + dist / 2 + 1, e, buf, cmp);
    merge(s, s + dist / 2 + 1, s + dist / 2 + 1, e, buf, cmp);
    for(long long i = 0; i < dist; i++) *(s + i) = *(buf + i);
}

template <typename T>
void sort_(T* s, T* e, T* buf) {
    long long dist = e - s;
    if(dist <= 16) {
        naive_sort(s, e);
        return;
    }
    sort_(s, s + dist / 2 + 1, buf);
    sort_(s + dist / 2 + 1, e, buf);
    merge(s, s + dist / 2 + 1, s + dist / 2 + 1, e, buf);
    for(long long i = 0; i < dist; i++) *(s + i) = *(buf + i);
}

template <typename T> inline void sort(T* s, T* e) {
    T* buffer = new T[e - s]();
    sort_(s, e, buffer);
    delete[] buffer;
}
template <typename T, typename Compare> inline void sort(T* s, T* e, Compare cmp) {
    T* buffer = new T[e - s]();
    sort_(s, e, buffer, cmp);
    delete[] buffer;
}


int main() {
    int n = readInt(true);
    auto arr = readArr<int>(n);
    sort(arr.begin(), arr.end());
    for(int i : arr) cout << i << ' ';
}
