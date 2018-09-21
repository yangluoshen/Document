#include "IncludeAll.h"

vint CheckTokens(List<RegexToken>& tokens)
{
	FOREACH(RegexToken, token, tokens)
	{
		switch ((CppTokens)token.token)
		{
		case CppTokens::LBRACE:
			TEST_ASSERT(token.length == 1 && *token.reading == L'{');
			break;
		case CppTokens::RBRACE:
			TEST_ASSERT(token.length == 1 && *token.reading == L'}');
			break;
		case CppTokens::LBRACKET:
			TEST_ASSERT(token.length == 1 && *token.reading == L'[');
			break;
		case CppTokens::RRACKET:
			TEST_ASSERT(token.length == 1 && *token.reading == L']');
			break;
		case CppTokens::LPARENTHESIS:
			TEST_ASSERT(token.length == 1 && *token.reading == L'(');
			break;
		case CppTokens::RPARENTHESIS:
			TEST_ASSERT(token.length == 1 && *token.reading == L')');
			break;
		case CppTokens::LT:
			TEST_ASSERT(token.length == 1 && *token.reading == L'<');
			break;
		case CppTokens::GT:
			TEST_ASSERT(token.length == 1 && *token.reading == L'>');
			break;
		case CppTokens::EQ:
			TEST_ASSERT(token.length == 1 && *token.reading == L'=');
			break;
		case CppTokens::NOT:
			TEST_ASSERT(token.length == 1 && *token.reading == L'!');
			break;
		case CppTokens::PERCENT:
			TEST_ASSERT(token.length == 1 && *token.reading == L'%');
			break;
		case CppTokens::COLON:
			TEST_ASSERT(token.length == 1 && *token.reading == L':');
			break;
		case CppTokens::SEMICOLON:
			TEST_ASSERT(token.length == 1 && *token.reading == L';');
			break;
		case CppTokens::DOT:
			TEST_ASSERT(token.length == 1 && *token.reading == L'.');
			break;
		case CppTokens::QUESTIONMARK:
			TEST_ASSERT(token.length == 1 && *token.reading == L'?');
			break;
		case CppTokens::COMMA:
			TEST_ASSERT(token.length == 1 && *token.reading == L',');
			break;
		case CppTokens::MUL:
			TEST_ASSERT(token.length == 1 && *token.reading == L'*');
			break;
		case CppTokens::ADD:
			TEST_ASSERT(token.length == 1 && *token.reading == L'+');
			break;
		case CppTokens::SUB:
			TEST_ASSERT(token.length == 1 && *token.reading == L'-');
			break;
		case CppTokens::DIV:
			TEST_ASSERT(token.length == 1 && *token.reading == L'/');
			break;
		case CppTokens::XOR:
			TEST_ASSERT(token.length == 1 && *token.reading == L'^');
			break;
		case CppTokens::AND:
			TEST_ASSERT(token.length == 1 && *token.reading == L'&');
			break;
		case CppTokens::OR:
			TEST_ASSERT(token.length == 1 && *token.reading == L'|');
			break;
		case CppTokens::REVERT:
			TEST_ASSERT(token.length == 1 && *token.reading == L'~');
			break;
		case CppTokens::SHARP:
			TEST_ASSERT(token.length == 1 && *token.reading == L'#');
			break;
		case CppTokens::INT:
			{
				auto reading = token.reading;
				auto length = token.length;

				if (length > 2 && (reading[length - 2] == L'u' || reading[length - 2] == L'U'))
				{
					TEST_ASSERT(reading[length - 1] == L'l' || reading[length - 1] == L'L');
					length -= 2;
				}
				else if (length > 1)
				{
					if (reading[length - 1] == L'u' || reading[length - 1] == L'U' || reading[length - 1] == L'l' || reading[length - 1] == L'L')
					{
						length -= 1;
					}
				}

				for (vint i = 0; i < length; i++)
				{
					TEST_ASSERT((L'0' <= reading[i] && reading[i] <= L'9') || reading[i] == L'\'');
				}
			}
			break;
		case CppTokens::HEX:
			{
				auto reading = token.reading;
				auto length = token.length;
				TEST_ASSERT(length > 2);
				TEST_ASSERT(reading[0] == L'0' && (reading[1] == L'x' || reading[1] == L'X'));

				reading += 2;
				length -= 2;
				if (length > 2 && (reading[length - 2] == L'u' || reading[length - 2] == L'U'))
				{
					TEST_ASSERT(reading[length - 1] == L'l' || reading[length - 1] == L'L');
					length -= 2;
				}
				else if (length > 1)
				{
					if (reading[length - 1] == L'u' || reading[length - 1] == L'U' || reading[length - 1] == L'l' || reading[length - 1] == L'L')
					{
						length -= 1;
					}
				}

				for (vint i = 0; i < length; i++)
				{
					TEST_ASSERT((L'0' <= reading[i] && reading[i] <= L'9') || (L'a' <= reading[i] && reading[i] <= L'f') || (L'A' <= reading[i] && reading[i] <= L'F'));
				}
			}
			break;
		case CppTokens::BIN:
			{
				auto reading = token.reading;
				auto length = token.length;
				TEST_ASSERT(length > 2);
				TEST_ASSERT(reading[0] == L'0' && (reading[1] == L'b' || reading[1] == L'B'));

				reading += 2;
				length -= 2;
				if (length > 2 && (reading[length - 2] == L'u' || reading[length - 2] == L'U'))
				{
					TEST_ASSERT(reading[length - 1] == L'l' || reading[length - 1] == L'L');
					length -= 2;
				}
				else if (length > 1)
				{
					if (reading[length - 1] == L'u' || reading[length - 1] == L'U' || reading[length - 1] == L'l' || reading[length - 1] == L'L')
					{
						length -= 1;
					}
				}

				for (vint i = 0; i < length; i++)
				{
					TEST_ASSERT(reading[i] == L'0' || reading[i] == L'1');
				}
			}
			break;
		case CppTokens::FLOAT:
			{
				vint _1 = 0, _2 = 0;
				auto reading = token.reading;
				auto length = token.length;
				if (reading[length - 1] == L'f' || reading[length - 1] == L'F' || reading[length - 1] == L'l' || reading[length - 1] == L'L')
				{
					length--;
				}

				while (L'0' <= *reading && *reading <= L'9')
				{
					reading++;
					_1++;
				}
				TEST_ASSERT(*reading++ == L'.');
				while (L'0' <= *reading && *reading <= L'9')
				{
					reading++;
					_2++;
				}
				TEST_ASSERT(_1 > 0 || _2 > 0);

				if (*reading == L'e' || *reading == L'E')
				{
					reading++;
					if (*reading == L'+' || *reading == L'-') reading++;
					while (reading < token.reading + length)
					{
						TEST_ASSERT(L'0' <= *reading && *reading <= L'9');
						reading++;
					}
				}
				else
				{
					TEST_ASSERT(length == _1 + _2 + 1);
				}
			}
			break;
		case CppTokens::ID:
			{
				auto reading = token.reading;
				TEST_ASSERT((L'a' <= reading[0] && reading[0] <= L'z') || (L'A' <= reading[0] && reading[0] <= L'Z') || reading[0] == L'_');
				for (vint i = 1; i < token.length; i++)
				{
					TEST_ASSERT((L'0' <= reading[i] && reading[i] <= L'9') || (L'a' <= reading[i] && reading[i] <= L'z') || (L'A' <= reading[i] && reading[i] <= L'Z') || reading[i] == L'_');
				}
			}
			break;
		case CppTokens::STRING:
			{
				auto reading = token.reading;
				if (wcsncmp(reading, L"L\"", 2) == 0) reading += 2;
				else if (wcsncmp(reading, L"u\"", 2) == 0) reading += 2;
				else if (wcsncmp(reading, L"U\"", 2) == 0) reading += 2;
				else if (wcsncmp(reading, L"u8\"", 3) == 0) reading += 3;
				else if (wcsncmp(reading, L"\"", 1) == 0) reading += 1;
				else TEST_ASSERT(false);

				while (*reading != L'\"')
				{
					TEST_ASSERT(*reading != 0);
					reading += (*reading == L'\\' ? 2 : 1);
				}
				TEST_ASSERT(token.length == (vint)(reading - token.reading + 1));
			}
			break;
		case CppTokens::CHAR:
			{
				auto reading = token.reading;
				if (wcsncmp(reading, L"L'", 2) == 0) reading += 2;
				else if (wcsncmp(reading, L"u'", 2) == 0) reading += 2;
				else if (wcsncmp(reading, L"U'", 2) == 0) reading += 2;
				else if (wcsncmp(reading, L"u8'", 3) == 0) reading += 3;
				else if (wcsncmp(reading, L"'", 1) == 0) reading += 1;
				else TEST_ASSERT(false);

				while (*reading != L'\'')
				{
					TEST_ASSERT(*reading != 0);
					reading += (*reading == L'\\' ? 2 : 1);
				}
				TEST_ASSERT(token.length == (vint)(reading - token.reading + 1));
			}
			break;
		case CppTokens::SPACE:
			{
				for (vint i = 0; i < token.length; i++)
				{
					auto c = token.reading[i];
					TEST_ASSERT(c == L' ' || c == L'\t' || c == L'\r' || c == L'\n' || c == L'\v' || c == L'\f');
				}
			}
			break;
		case CppTokens::DOCUMENT:
			{
				TEST_ASSERT(token.length >= 3);
				TEST_ASSERT(token.reading[0] == L'/');
				TEST_ASSERT(token.reading[1] == L'/');
				TEST_ASSERT(token.reading[2] == L'/');
				TEST_ASSERT(token.reading[token.length] == L'\r' || token.reading[token.length] == L'\n' || token.reading[token.length] == 0);
			}
			break;
		case CppTokens::COMMENT1:
			{
				TEST_ASSERT(token.length >= 2);
				TEST_ASSERT(token.reading[0] == L'/');
				TEST_ASSERT(token.reading[1] == L'/');
				TEST_ASSERT(token.reading[2] != L'/');
				TEST_ASSERT(token.reading[token.length] == L'\r' || token.reading[token.length] == L'\n' || token.reading[token.length] == 0);
			}
			break;
		case CppTokens::COMMENT2:
			{
				TEST_ASSERT(token.length >= 4);
				TEST_ASSERT(token.reading[0] == L'/');
				TEST_ASSERT(token.reading[1] == L'*');
				TEST_ASSERT(token.reading[token.length - 2] == L'*');
				TEST_ASSERT(token.reading[token.length - 1] == L'/');
				TEST_ASSERT(wcsstr(token.reading, L"*/") == token.reading + token.length - 2);
			}
			break;
		default:
			TEST_ASSERT(false);
		}
	}
	return tokens.Count();
}

TEST_CASE(TestLexer_Punctuators)
{
	WString input = LR"({}[]()<>=!%:;.?,*+-/^&|~#)";
	List<RegexToken> tokens;
	CreateCppLexer()->Parse(input).ReadToEnd(tokens);
	TEST_ASSERT(CheckTokens(tokens) == 25);
}

TEST_CASE(TestLexer_Numbers)
{
	WString input = LR"(
123
123'123'123u
123l
123'123'123UL
0x12345678
0xDEADBEEFu
0X12345678l
0XDEADBEEFUL
0b00001111
0b11110000u
0B00001111l
0B11110000UL
123.456
123.f
.456l
123.456e10
123.e+10F
.456e-10L
)";
	List<RegexToken> tokens;
	CreateCppLexer()->Parse(input).ReadToEnd(tokens);
	TEST_ASSERT(CheckTokens(tokens) == 37);
}

TEST_CASE(TestLexer_Strings)
{
	WString input = LR"(
"abc"
L"\"\"xxxx\"\""
u"xxxx\"\"xxxx"
U"\"\"xxxx\"\""
u8"xxxx\"\"xxxx"
'a'
L'\''
u'\''
U'\''
u8'\''
)";
	List<RegexToken> tokens;
	CreateCppLexer()->Parse(input).ReadToEnd(tokens);
	TEST_ASSERT(CheckTokens(tokens) == 21);
}

TEST_CASE(TestLexer_Comments)
{
	WString input = LR"(
//
//xxxxx
///
///xxxxx
/**/
/********/
/* xxxxx */
/* xx*xx */
/* xx**x */
/* x***x */
)";
	List<RegexToken> tokens;
	CreateCppLexer()->Parse(input).ReadToEnd(tokens);
	TEST_ASSERT(CheckTokens(tokens) == 21);
}

TEST_CASE(TestLexer_HelloWorld)
{
	WString input = LR"(
using namespace std;

int main()
{
	cout << "Hello, world!" << endl;
}
)";
	List<RegexToken> tokens;
	CreateCppLexer()->Parse(input).ReadToEnd(tokens);
	TEST_ASSERT(CheckTokens(tokens) == 31);
}

TEST_CASE(TestLexer_GacUI_Input)
{
	FilePath inputPath = L"../../../.Output/Import/Preprocessed.txt";
	TEST_ASSERT(inputPath.IsFile());

	wchar_t* buffer = ReadBigFile(inputPath);

	List<RegexToken> tokens;
	CreateCppLexer()->Parse(WString(buffer, false)).ReadToEnd(tokens);
	CheckTokens(tokens);
	delete[] buffer;
}

TEST_CASE(TestLexer_Reader)
{
	WString input = LR"(
using namespace std;

/// <summary>The main function.</summary>
/// <returns>This value is not used.</returns>
int main()
{
	cout << "Hello, world!" << endl;
}
)";
	const wchar_t* output[] = {
		L"using", L"namespace", L"std", L";",
		L"/// <summary>The main function.</summary>",
		L"/// <returns>This value is not used.</returns>",
		L"int", L"main", L"(", L")",
		L"{",
		L"cout", L"<", L"<", L"\"Hello, world!\"", L"<", L"<", L"end", L";",
		L"}",
	};


	CppTokenReader reader(input);
}