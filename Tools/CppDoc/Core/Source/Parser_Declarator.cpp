#include "Parser.h"
#include "Ast_Type.h"
#include "Ast_Decl.h"

template<typename T>
class ReplaceOutOfDeclaratorTypeVisitor : public Object, public virtual ITypeVisitor
{
public:
	Ptr<T>						createdType;
	Ptr<Type>					typeToReplace;
	Func<Ptr<T>(Ptr<Type>)>		typeCreator;

	void Execute(Ptr<Type>& targetType)
	{
		if (targetType == typeToReplace)
		{
			createdType = typeCreator(targetType);
			targetType = createdType;
		}
		else
		{
			targetType->Accept(this);
		}
	}

	void Visit(IdType* self)override
	{
	}

	void Visit(PrimitiveType* self)override
	{
	}

	void Visit(ReferenceType* self)override
	{
		Execute(self->type);
	}

	void Visit(ArrayType* self)override
	{
		Execute(self->type);
	}

	void Visit(CallingConventionType* self)override
	{
		Execute(self->type);
	}

	void Visit(FunctionType* self)override
	{
		Execute(self->returnType);
	}

	void Visit(MemberType* self)override
	{
	}

	void Visit(DeclType* self)override
	{
	}

	void Visit(DecorateType* self)override
	{
		Execute(self->type);
	}

	void Visit(ChildType* self)override
	{
	}

	void Visit(GenericType* self)override
	{
	}

	void Visit(VariadicTemplateArgumentType* self)override
	{
	}
};



extern Ptr<Type> ParseShortType(ParsingArguments& pa, Ptr<CppTokenCursor>& cursor);
extern Ptr<Type> ParseLongType(ParsingArguments& pa, Ptr<CppTokenCursor>& cursor);

Ptr<Declarator> ParseShortDeclarator(ParsingArguments& pa, Ptr<Type> typeResult, DeclaratorRestriction dr, Ptr<CppTokenCursor>& cursor)
{
	while (SkipSpecifiers(cursor));

	if (TestToken(cursor, L"alignas"))
	{
		RequireToken(cursor, CppTokens::LPARENTHESIS);
		ParseExpr(pa, cursor);
		RequireToken(cursor, CppTokens::RPARENTHESIS);
		return ParseShortDeclarator(pa, typeResult, dr, cursor);
	}
	else if (TestToken(cursor, CppTokens::MUL))
	{
		TestToken(cursor, L"__ptr32") || TestToken(cursor, L"__ptr64");
		auto type = MakePtr<ReferenceType>();
		type->reference = CppReferenceType::Ptr;
		type->type = typeResult;
		return ParseShortDeclarator(pa, type, dr, cursor);
	}
	else if (TestToken(cursor, CppTokens::AND, CppTokens::AND))
	{
		auto type = MakePtr<ReferenceType>();
		type->reference = CppReferenceType::RRef;
		type->type = typeResult;
		return ParseShortDeclarator(pa, type, dr, cursor);
	}
	else if (TestToken(cursor, CppTokens::AND))
	{
		auto type = MakePtr<ReferenceType>();
		type->reference = CppReferenceType::LRef;
		type->type = typeResult;
		return ParseShortDeclarator(pa, type, dr, cursor);
	}
	else if (TestToken(cursor, CppTokens::CONSTEXPR))
	{
		auto type = typeResult.Cast<DecorateType>();
		if (!type)
		{
			type = MakePtr<DecorateType>();
			type->type = typeResult;
		}
		type->isConstExpr = true;
		return ParseShortDeclarator(pa, type, dr, cursor);
	}
	else if (TestToken(cursor, CppTokens::CONST))
	{
		auto type = typeResult.Cast<DecorateType>();
		if (!type)
		{
			type = MakePtr<DecorateType>();
			type->type = typeResult;
		}
		type->isConst = true;
		return ParseShortDeclarator(pa, type, dr, cursor);
	}
	else if (TestToken(cursor, CppTokens::VOLATILE))
	{
		auto type = typeResult.Cast<DecorateType>();
		if (!type)
		{
			type = MakePtr<DecorateType>();
			type->type = typeResult;
		}
		type->isVolatile = true;
		return ParseShortDeclarator(pa, type, dr, cursor);
	}

#define CALLING_CONVENTION_KEYWORD(TOKEN, NAME)\
	else if (!TestToken(cursor, CppTokens::TOKEN, CppTokens::LPARENTHESIS, false) && TestToken(cursor, CppTokens::TOKEN))\
	{\
		auto type = MakePtr<CallingConventionType>();\
		type->callingConvention = CppCallingConvention::NAME;\
		type->type = typeResult;\
		return ParseShortDeclarator(pa, type, dr, cursor);\
	}\

	CALLING_CONVENTION_KEYWORD(__CDECL, CDecl)
	CALLING_CONVENTION_KEYWORD(__CLRCALL, ClrCall)
	CALLING_CONVENTION_KEYWORD(__STDCALL, StdCall)
	CALLING_CONVENTION_KEYWORD(__FASTCALL, FastCall)
	CALLING_CONVENTION_KEYWORD(__THISCALL, ThisCall)
	CALLING_CONVENTION_KEYWORD(__VECTORCALL, VectorCall)

#undef CALLING_CONVENTION_KEYWORD

	else
	{
		{
			auto oldCursor = cursor;
			try
			{
				auto type = ParseShortType(pa, cursor);
				cursor = oldCursor;
			}
			catch (const StopParsingException&)
			{
				goto CHECK_CPP_NAME;
			}

			cursor = oldCursor;
			throw StopParsingException(cursor);
		}
	CHECK_CPP_NAME:
		{
			auto declarator = MakePtr<Declarator>();
			declarator->type = typeResult;
			if (dr != DeclaratorRestriction::Zero)
			{
				if (!ParseCppName(declarator->name, cursor))
				{
					if (dr == DeclaratorRestriction::One)
					{
						throw StopParsingException(cursor);
					}
				}
			}
			return declarator;
		}
	}
}

Ptr<Declarator> ParseLongDeclarator(ParsingArguments& pa, Ptr<Type> typeResult, DeclaratorRestriction dr, Ptr<CppTokenCursor>& cursor)
{
	auto declarator = ParseShortDeclarator(pa, typeResult, dr, cursor);
	auto targetType = declarator->type;

	{
		auto oldCursor = cursor;
		if (!declarator->name)
		{
			if (TestToken(cursor, CppTokens::LPARENTHESIS) && TestToken(cursor, CppTokens::RPARENTHESIS))
			{
				cursor = oldCursor;
				goto GIVE_UP;
			}

			cursor = oldCursor;
			if (TestToken(cursor, CppTokens::LPARENTHESIS) && TestToken(cursor, CppTokens::TYPE_VOID) && TestToken(cursor, CppTokens::RPARENTHESIS))
			{
				cursor = oldCursor;
				goto GIVE_UP;
			}

			cursor = oldCursor;
			if (TestToken(cursor, CppTokens::LPARENTHESIS))
			{
				try
				{
					declarator = ParseLongDeclarator(pa, targetType, dr, cursor);
				}
				catch (const StopParsingException&)
				{
					cursor = oldCursor;
					goto GIVE_UP;
				}

				RequireToken(cursor, CppTokens::RPARENTHESIS);
			}
		}
	}
GIVE_UP:

	if (TestToken(cursor, CppTokens::LBRACKET, false))
	{
		while (true)
		{
			if (TestToken(cursor, CppTokens::LBRACKET))
			{
				ReplaceOutOfDeclaratorTypeVisitor<ArrayType> replacer;
				{
					replacer.typeToReplace = targetType;
					replacer.typeCreator = [](Ptr<Type> typeToReplace)
					{
						auto type = MakePtr<ArrayType>();
						type->type = typeToReplace;
						return type;
					};

					replacer.Execute(declarator->type);
					targetType = replacer.createdType;
				}

				if (!TestToken(cursor, CppTokens::RBRACKET))
				{
					replacer.createdType->expr = ParseExpr(pa, cursor);
					RequireToken(cursor, CppTokens::RBRACKET);
				}
			}
			else
			{
				return declarator;
			}
		}
	}

	bool hasCallingConvention = false;
	CppCallingConvention callingConvention;

#define CALLING_CONVENTION_KEYWORD(TOKEN, NAME)\
	if (TestToken(cursor, CppTokens::TOKEN, CppTokens::LPARENTHESIS, false))\
	{\
		hasCallingConvention = true;\
		callingConvention = CppCallingConvention::NAME;\
		cursor = cursor->Next();\
	}\
	else\

	CALLING_CONVENTION_KEYWORD(__CDECL, CDecl)
	CALLING_CONVENTION_KEYWORD(__CLRCALL, ClrCall)
	CALLING_CONVENTION_KEYWORD(__STDCALL, StdCall)
	CALLING_CONVENTION_KEYWORD(__FASTCALL, FastCall)
	CALLING_CONVENTION_KEYWORD(__THISCALL, ThisCall)
	CALLING_CONVENTION_KEYWORD(__VECTORCALL, VectorCall)

#undef CALLING_CONVENTION_KEYWORD
	;

	if (TestToken(cursor, CppTokens::LPARENTHESIS))
	{
		Ptr<FunctionType> type;
		if (hasCallingConvention)
		{
			if (declarator->type != targetType)
			{
				throw StopParsingException(cursor);
			}

			type = MakePtr<FunctionType>();
			type->returnType = declarator->type;

			auto ccType = MakePtr<CallingConventionType>();
			ccType->callingConvention = callingConvention;
			ccType->type = type;
			declarator->type = ccType;
		}
		else
		{
			ReplaceOutOfDeclaratorTypeVisitor<FunctionType> replacer;
			{
				replacer.typeToReplace = targetType;
				replacer.typeCreator = [](Ptr<Type> typeToReplace)
				{
					auto type = MakePtr<FunctionType>();
					type->returnType = typeToReplace;
					return type;
				};

				replacer.Execute(declarator->type);
			}
			type = replacer.createdType;
		}

		{
			auto oldCursor = cursor;
			if (TestToken(cursor, CppTokens::TYPE_VOID) && TestToken(cursor, CppTokens::RPARENTHESIS))
			{
				goto FINISH_PARAMETER_LIST;
			}
			else
			{
				cursor = oldCursor;
			}
		}

		while (!TestToken(cursor, CppTokens::RPARENTHESIS))
		{
			{
				List<Ptr<Declarator>> declarators;
				ParseDeclarator(pa, DeclaratorRestriction::Optional, InitializerRestriction::Optional, cursor, declarators);
				if (declarators.Count() != 1)
				{
					throw StopParsingException(cursor);
				}
				type->parameters.Add(declarators[0]);
			}

			if (TestToken(cursor, CppTokens::RPARENTHESIS))
			{
				break;
			}
			else
			{
				RequireToken(cursor, CppTokens::COMMA);
			}
		}
	FINISH_PARAMETER_LIST:

		while (true)
		{
			if (TestToken(cursor, CppTokens::CONSTEXPR))
			{
				type->qualifierConstExpr = true;
			}
			else if (TestToken(cursor, CppTokens::CONST))
			{
				type->qualifierConst = true;
			}
			else if (TestToken(cursor, CppTokens::VOLATILE))
			{
				type->qualifierVolatile = true;
			}
			else if (TestToken(cursor, CppTokens::AND, CppTokens::AND))
			{
				type->qualifierRRef = true;
			}
			else if (TestToken(cursor, CppTokens::AND))
			{
				type->qualifierLRef = true;
			}
			else if (TestToken(cursor, CppTokens::OVERRIDE))
			{
				type->decoratorOverride = true;
			}
			else if (TestToken(cursor, CppTokens::SUB, CppTokens::GT))
			{
				if (auto primitiveType = type->returnType.Cast<PrimitiveType>())
				{
					if (primitiveType->primitive == CppPrimitiveType::_auto)
					{
						goto CONTINUE_RETURN_TYPE;
					}
				}
				throw StopParsingException(cursor);
			CONTINUE_RETURN_TYPE:

				List<Ptr<Declarator>> declarators;
				ParseDeclarator(pa, DeclaratorRestriction::Zero, InitializerRestriction::Zero, cursor, declarators);
				if (declarators.Count() != 1)
				{
					throw StopParsingException(cursor);
				}
				type->decoratorReturnType = declarators[0]->type;
			}
			else if (TestToken(cursor, CppTokens::NOEXCEPT))
			{
				type->decoratorNoExcept = true;
			}
			else if (TestToken(cursor, CppTokens::THROW))
			{
				type->decoratorThrow = true;

				RequireToken(cursor, CppTokens::LPARENTHESIS);
				while (!TestToken(cursor, CppTokens::RPARENTHESIS))
				{
					List<Ptr<Declarator>> declarators;
					ParseDeclarator(pa, DeclaratorRestriction::Zero, InitializerRestriction::Zero, cursor, declarators);
					if (declarators.Count() != 1)
					{
						throw StopParsingException(cursor);
					}
					type->exceptions.Add(declarators[0]->type);

					if (TestToken(cursor, CppTokens::RPARENTHESIS))
					{
						break;
					}
					else
					{
						RequireToken(cursor, CppTokens::COMMA);
					}
				}
			}
			else
			{
				return declarator;
			}
		}
	}

	return declarator;
}

Ptr<Initializer> ParseInitializer(ParsingArguments& pa, Ptr<CppTokenCursor>& cursor)
{
	auto initializer = MakePtr<Initializer>();

	if (TestToken(cursor, CppTokens::EQ))
	{
		initializer->initializerType = InitializerType::Equal;
	}
	else if (TestToken(cursor, CppTokens::LBRACE))
	{
		initializer->initializerType = InitializerType::Universal;
	}
	else if (TestToken(cursor, CppTokens::LPARENTHESIS))
	{
		initializer->initializerType = InitializerType::Constructor;
	}

	while (true)
	{
		initializer->arguments.Add(ParseExpr(pa, cursor));

		if (initializer->initializerType == InitializerType::Equal)
		{
			break;
		}
		else if (!TestToken(cursor, CppTokens::COMMA))
		{
			switch (initializer->initializerType)
			{
			case InitializerType::Universal:
				RequireToken(cursor, CppTokens::RBRACE);
				break;
			case InitializerType::Constructor:
				RequireToken(cursor, CppTokens::RPARENTHESIS);
				break;
			}
			break;
		}
	}

	return initializer;
}

void ParseDeclarator(ParsingArguments& pa, DeclaratorRestriction dr, InitializerRestriction ir, Ptr<CppTokenCursor>& cursor, List<Ptr<Declarator>>& declarators)
{
	Ptr<Type> typeResult = ParseLongType(pa, cursor);

	auto itemDr = dr == DeclaratorRestriction::Many ? DeclaratorRestriction::One : dr;
	while(true)
	{
		auto declarator = ParseLongDeclarator(pa, typeResult, itemDr, cursor);
		if (ir == InitializerRestriction::Optional)
		{
			if (TestToken(cursor, CppTokens::EQ, false) || TestToken(cursor, CppTokens::LBRACE, false) || TestToken(cursor, CppTokens::LPARENTHESIS, false))
			{
				declarator->initializer = ParseInitializer(pa, cursor);
			}
		}
		declarators.Add(declarator);

		if (dr != DeclaratorRestriction::Many)
		{
			break;
		}
		else if (!TestToken(cursor, CppTokens::COMMA))
		{
			break;
		}
	}
}