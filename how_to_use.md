# koiLib
코이스터디의 데이터를 수정하기 위한 라이브러리  

ac 코드를 기반으로 벨리데이터 및 입력 파일 포매터를 자동 생성하는 것이 목표입니다.

아래 내용은 v1.31을 기반으로 작성되었습니다.
## 사용법

koiLib.cpp 파일에서 `#pragma endregion` 이후에 문제 풀이 코드를 작성하면 됩니다.
이때 유의할 점은 입력을 아래에 설명된 함수들을 사용해 받고, 출력은 cout으로 해야 한다는 점입니다.
`printf`, `puts`, `scanf` 등 C-style 입출력은 사용할 수 없습니다.

작성한 코드는 `VALIDATOR`를 `true`로 설정하면 벨리데이터 (겸 ac 코드), `FORMATTER`를 `true`로 설정하면 포매터가 됩니다.  

벨리데이터는 코이스터디에 제출하면 입력 형식이 잘못된 경우에 런타임 에러를 발생시키고, 그에 대한 설명을 cerr에 출력합니다.  
세부 항목의 검사 여부를 켜고 끄는 기능도 지원합니다. 자세한 내용은 코드에 주석으로 설명되어 있습니다.  

포매터는 입력 파일을 입력으로 받아, 올바른 형식의 입력 파일을 출력합니다.

팁) `namespace koi_lib` 코드블럭이나 `#pragma region`을 접으면 편합니다.

## 입력 함수

- `read{Type}(readEof: bool = false)`
  - 값 하나를 입력받는 함수입니다.  
    값 뒤에 공백문자가 와야 하는 경우 `readInt()`, 줄바꿈이 와야 하는 경우 `readInt(true)`를 사용합니다.
  - `readInt`, `readLong`, `readDouble`, `readLDouble`, `readStr`, `readChar`
  

- `read{Type}(end: char)`
  - 값 하나를 입력받는 함수입니다. 값 뒤에 와야 할 문자를 명시할 수 있습니다.
  

- `read<T>(bool | char)`
  - 위 두 함수와 같은 동작을 합니다.
  

- `read{Type}s<arg_cnt>()`
  - 값 여러 개를 입력받는 함수입니다. 
  - 값들은 공백으로 구분되어 있고, 마지막엔 줄바꿈이 주어질 때 사용합니다.
  - `std::array<type, arg_cnt>`를 반환합니다.
  - `readInts`, `readLongs`, `readDoubles`, `readLDoubles`, `readStrs`
  

- `read<T1, T2, ...>`
  - 서로 다른 타입의 값 여러 개를 입력받는 함수입니다.
  - 값들은 공백으로 구분되어 있고, 마지막엔 줄바꿈이 주어질 때 사용합니다.
  - `std::tuple<T1, T2, ...>`를 반환합니다.
  - `int`, `long long`, `double`, `long double`, `std::string`, `char`를 지원합니다.
  

- `readArr<Type = long long>(n: int, sep: char = ' ')`
  - 공백으로 구분되어 주어지는 크기 n의 배열을 입력받을 때 사용합니다.
  - 공백이 아닌 다른 문자로 구분되어 주어질 경우, `sep` 인자에 해당 문자를 넘기면 됩니다.
  - 지원하는 타입은 위와 동일.
  

- `get(args...)`
  - 인자로 주어진 변수들에 입력을 받습니다.
  - 값들은 공백으로 구분되어 있고, 마지막엔 줄바꿈이 주어질 때 사용합니다.
  - `auto [a, b, c] = read<Ta, Tb, Tc>;` 와 `Ta a; Tb b; Tc c; get(a, b, c);`가 동일
  

- `readEof()`
  - 입력 형식 확인을 위해 end-of-file을 입력받을 때 사용합니다.
  - 입력을 모두 받은 시점에 호출해주면, 남은 연산을 생략할 수 있어 입력 파일 포매터의 성능이 향상됩니다.