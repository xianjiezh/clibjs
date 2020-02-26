//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <fstream>
#include <algorithm>
#include "cjsparser.h"
#include "cjsast.h"

#define REPORT_ERROR 0
#define REPORT_ERROR_FILE "parsing.log"

#define TRACE_PARSING 0
#define DUMP_PDA 0
#define DUMP_PDA_FILE "PDA.log"
#define DEBUG_AST 0
#define CHECK_AST 0

namespace clib {

    ast_node *cjsparser::parse(const std::string &str, csemantic *s) {
        semantic = s;
        lexer = std::make_unique<cjslexer>();
        lexer->input(str);
        ast = std::make_unique<cjsast>();
        current = nullptr;
        // 清空AST
        ast->reset();
        // 产生式
        if (unit.get_pda().empty())
            gen();
        // 语法分析（递归下降）
        program();
        return ast->get_root();
    }

    ast_node *cjsparser::root() const {
        return ast->get_root();
    }

    void cjsparser::clear_ast() {
        ast.reset();
    }

    void cjsparser::next() {
        current = &lexer->get_current_unit();
        lexer->inc_index();
    }

    void cjsparser::gen() {
        // REFER: antlr/grammars-v4
        // URL: https://github.com/antlr/grammars-v4/blob/master/javascript/javascript/JavaScriptParser.g4
#define DEF_LEXER(name) auto &_##name = unit.token(name);
        DEF_LEXER(END)
        DEF_LEXER(NUMBER)
        DEF_LEXER(ID)
        DEF_LEXER(REGEX)
        DEF_LEXER(STRING)
        // ---
        DEF_LEXER(K_NEW)
        DEF_LEXER(K_VAR)
        DEF_LEXER(K_LET)
        DEF_LEXER(K_FUNCTION)
        DEF_LEXER(K_IF)
        DEF_LEXER(K_ELSE)
        DEF_LEXER(K_FOR)
        DEF_LEXER(K_WHILE)
        DEF_LEXER(K_IN)
        DEF_LEXER(K_DO)
        DEF_LEXER(K_BREAK)
        DEF_LEXER(K_CONTINUE)
        DEF_LEXER(K_RETURN)
        DEF_LEXER(K_SWITCH)
        DEF_LEXER(K_DEFAULT)
        DEF_LEXER(K_CASE)
        DEF_LEXER(K_NULL)
        DEF_LEXER(K_TRUE)
        DEF_LEXER(K_FALSE)
        DEF_LEXER(K_INSTANCEOF)
        DEF_LEXER(K_TYPEOF)
        DEF_LEXER(K_VOID)
        DEF_LEXER(K_DELETE)
        DEF_LEXER(K_CLASS)
        DEF_LEXER(K_THIS)
        DEF_LEXER(K_SUPER)
        DEF_LEXER(K_WITH)
        DEF_LEXER(K_TRY)
        DEF_LEXER(K_THROW)
        DEF_LEXER(K_CATCH)
        DEF_LEXER(K_FINALLY)
        DEF_LEXER(K_DEBUGGER)
        DEF_LEXER(K_GET)
        DEF_LEXER(K_SET)
        // ---
        DEF_LEXER(T_ADD)
        DEF_LEXER(T_SUB)
        DEF_LEXER(T_MUL)
        DEF_LEXER(T_DIV)
        DEF_LEXER(T_MOD)
        DEF_LEXER(T_POWER)
        DEF_LEXER(T_INC)
        DEF_LEXER(T_DEC)
        DEF_LEXER(T_ASSIGN)
        DEF_LEXER(T_ASSIGN_ADD)
        DEF_LEXER(T_ASSIGN_SUB)
        DEF_LEXER(T_ASSIGN_MUL)
        DEF_LEXER(T_ASSIGN_DIV)
        DEF_LEXER(T_ASSIGN_MOD)
        DEF_LEXER(T_ASSIGN_LSHIFT)
        DEF_LEXER(T_ASSIGN_RSHIFT)
        DEF_LEXER(T_ASSIGN_URSHIFT)
        DEF_LEXER(T_ASSIGN_AND)
        DEF_LEXER(T_ASSIGN_OR)
        DEF_LEXER(T_ASSIGN_XOR)
        DEF_LEXER(T_ASSIGN_POWER)
        DEF_LEXER(T_LESS)
        DEF_LEXER(T_LESS_EQUAL)
        DEF_LEXER(T_GREATER)
        DEF_LEXER(T_GREATER_EQUAL)
        DEF_LEXER(T_EQUAL)
        DEF_LEXER(T_FEQUAL)
        DEF_LEXER(T_NOT_EQUAL)
        DEF_LEXER(T_FNOT_EQUAL)
        DEF_LEXER(T_LOG_NOT)
        DEF_LEXER(T_LOG_AND)
        DEF_LEXER(T_LOG_OR)
        DEF_LEXER(T_BIT_NOT)
        DEF_LEXER(T_BIT_AND)
        DEF_LEXER(T_BIT_OR)
        DEF_LEXER(T_BIT_XOR)
        DEF_LEXER(T_DOT)
        DEF_LEXER(T_COMMA)
        DEF_LEXER(T_SEMI)
        DEF_LEXER(T_COLON)
        DEF_LEXER(T_QUERY)
        DEF_LEXER(T_LSHIFT)
        DEF_LEXER(T_RSHIFT)
        DEF_LEXER(T_URSHIFT)
        DEF_LEXER(T_LPARAN)
        DEF_LEXER(T_RPARAN)
        DEF_LEXER(T_LSQUARE)
        DEF_LEXER(T_RSQUARE)
        DEF_LEXER(T_LBRACE)
        DEF_LEXER(T_RBRACE)
        DEF_LEXER(T_COALESCE)
        DEF_LEXER(T_SHARP)
        DEF_LEXER(T_ELLIPSIS)
        DEF_LEXER(T_ARROW)
#undef DEF_LEXER
#define DEF_RULE(name) auto &name = unit.rule(#name, c_##name);
#define DEF_RULE_ATTR(name, attr) auto &name = unit.rule(#name, c_##name, attr);
#define DEF_RULE_NOT_GREED(name) DEF_RULE_ATTR(name, r_not_greed)
#define DEF_RULE_EXP(name) DEF_RULE_ATTR(name, r_exp)
        DEF_RULE(program)
        DEF_RULE(sourceElement)
        DEF_RULE(statement)
        DEF_RULE(block)
        DEF_RULE(statementList)
        DEF_RULE(declaration)
        DEF_RULE(variableStatement)
        DEF_RULE(variableDeclarationList)
        DEF_RULE(variableDeclaration)
        DEF_RULE(emptyStatement)
        DEF_RULE(expressionStatement)
        DEF_RULE(ifStatement)
        DEF_RULE(iterationStatement)
        DEF_RULE(continueStatement)
        DEF_RULE(breakStatement)
        DEF_RULE(returnStatement)
        DEF_RULE(withStatement)
        DEF_RULE(switchStatement)
        DEF_RULE(caseBlock)
        DEF_RULE(caseClauses)
        DEF_RULE(caseClause)
        DEF_RULE(defaultClause)
        DEF_RULE(labelledStatement)
        DEF_RULE(throwStatement)
        DEF_RULE(tryStatement)
        DEF_RULE(catchProduction)
        DEF_RULE(finallyProduction)
        DEF_RULE(debuggerStatement)
        DEF_RULE(functionDeclaration)
        DEF_RULE(classDeclaration)
        DEF_RULE(classTail)
        DEF_RULE(classElement)
        DEF_RULE(classElements)
        DEF_RULE(methodDefinition)
        DEF_RULE(formalParameterList)
        DEF_RULE(formalParameterArg)
        DEF_RULE(functionBody)
        DEF_RULE(sourceElements)
        DEF_RULE(arrayLiteral)
        DEF_RULE(elementList)
        DEF_RULE(arrayElement)
        DEF_RULE(objectLiteral)
        DEF_RULE(propertyAssignment)
        DEF_RULE(propertyAssignments)
        DEF_RULE(propertyName)
        DEF_RULE(arguments)
        DEF_RULE(argument)
        DEF_RULE(expressionSequence)
        DEF_RULE_EXP(singleExpression)
        DEF_RULE(assignable)
        DEF_RULE(anonymousFunction)
        DEF_RULE(arrowFunctionParameters)
        DEF_RULE(arrowFunctionBody)
        DEF_RULE(literal)
        DEF_RULE(numericLiteral)
        DEF_RULE(identifierName)
        DEF_RULE(getter)
        DEF_RULE(setter)
        DEF_RULE(eos)
        DEF_RULE(propertyExpressionAssignment)
        DEF_RULE(computedPropertyExpressionAssignment)
        DEF_RULE(propertyGetter)
        DEF_RULE(propertySetter)
        DEF_RULE(propertyShorthand)
        DEF_RULE(functionDecl)
        DEF_RULE(anoymousFunctionDecl)
        DEF_RULE(arrowFunction)
        DEF_RULE_EXP(functionExpression)
        DEF_RULE_EXP(classExpression)
        DEF_RULE_EXP(memberIndexExpression)
        DEF_RULE_EXP(memberDotExpression)
        DEF_RULE_EXP(argumentsExpression)
        DEF_RULE_EXP(newExpression)
        DEF_RULE_EXP(postIncrementExpression)
        DEF_RULE_EXP(postDecreaseExpression)
        DEF_RULE_EXP(deleteExpression)
        DEF_RULE_EXP(voidExpression)
        DEF_RULE_EXP(typeofExpression)
        DEF_RULE_EXP(preIncrementExpression)
        DEF_RULE_EXP(preDecreaseExpression)
        DEF_RULE_EXP(unaryPlusExpression)
        DEF_RULE_EXP(unaryMinusExpression)
        DEF_RULE_EXP(bitNotExpression)
        DEF_RULE_EXP(notExpression)
        DEF_RULE_EXP(powerExpression)
        DEF_RULE_EXP(multiplicativeExpression)
        DEF_RULE_EXP(additiveExpression)
        DEF_RULE_EXP(coalesceExpression)
        DEF_RULE_EXP(bitShiftExpression)
        DEF_RULE_EXP(relationalExpression)
        DEF_RULE_EXP(instanceofExpression)
        DEF_RULE_EXP(inExpression)
        DEF_RULE_EXP(equalityExpression)
        DEF_RULE_EXP(bitAndExpression)
        DEF_RULE_EXP(bitXOrExpression)
        DEF_RULE_EXP(bitOrExpression)
        DEF_RULE_EXP(logicalAndExpression)
        DEF_RULE_EXP(logicalOrExpression)
        DEF_RULE_EXP(ternaryExpression)
        DEF_RULE_EXP(assignmentExpression)
        DEF_RULE_EXP(assignmentOperatorExpression)
        DEF_RULE_EXP(thisExpression)
        DEF_RULE_EXP(identifierExpression)
        DEF_RULE_EXP(superExpression)
        DEF_RULE_EXP(literalExpression)
        DEF_RULE_EXP(arrayLiteralExpression)
        DEF_RULE_EXP(objectLiteralExpression)
        DEF_RULE_EXP(parenthesizedExpression)
#undef DEF_RULE
#undef DEF_RULE_NOT_GREED
#undef DEF_RULE_EXP
        program = sourceElements;
        declaration = variableStatement
                      | classDeclaration
                      | functionDeclaration;
        variableStatement = _K_VAR + variableDeclarationList + *eos;
        variableDeclarationList = variableDeclaration + *(~_T_COMMA + variableDeclarationList);
        variableDeclaration = assignable + *(_T_ASSIGN + singleExpression);
        emptyStatement = _T_SEMI;
        expressionStatement = expressionSequence + *eos;
        ifStatement = _K_IF + ~_T_LPARAN + expressionSequence + ~_T_RPARAN + statement + *(_K_ELSE + statement);
        iterationStatement = _K_WHILE + ~_T_LPARAN + expressionSequence + ~_T_LPARAN + statement
                             | _K_DO + statement + _K_WHILE + ~_T_LPARAN + expressionSequence + ~_T_LPARAN + *eos
                             | _K_FOR + ~_T_LPARAN + *(expressionSequence | variableDeclarationList) + _T_SEMI +
                               *expressionSequence + _T_SEMI + *expressionSequence + ~_T_LPARAN + statement
                             | _K_FOR + ~_T_LPARAN + *(singleExpression | variableDeclarationList) + _K_IN +
                               *expressionSequence + ~_T_LPARAN + statement;
        continueStatement = _K_CONTINUE + *_ID + *eos;
        breakStatement = _K_BREAK + *_ID + *eos;
        returnStatement = _K_RETURN + *expressionSequence + *eos;
        withStatement = _K_WITH + ~_T_LPARAN + expressionSequence + ~_T_LPARAN + statement;
        switchStatement = _K_SWITCH + ~_T_LPARAN + expressionSequence + ~_T_LPARAN + caseBlock;
        caseBlock = ~_T_LBRACE + *caseClauses + *(defaultClause + *caseClauses) + ~_T_RBRACE;
        caseClauses = caseClause + *caseClauses;
        caseClause = _K_CASE + expressionSequence + _T_ASSIGN + *statementList;
        defaultClause = _K_DEFAULT + _T_ASSIGN + *statementList;
        labelledStatement = _ID + _T_ASSIGN + statement;
        throwStatement = _K_THROW + expressionSequence + *eos;
        tryStatement = _K_TRY + block + (catchProduction + *finallyProduction | finallyProduction);
        catchProduction = _K_CATCH + *(~_T_LPARAN + *assignable + ~_T_LPARAN) + block;
        finallyProduction = _K_FINALLY + block;
        debuggerStatement = _K_DEBUGGER + *eos;
        functionDeclaration = _K_FUNCTION + _ID + ~_T_LPARAN + *formalParameterList + ~_T_RPARAN +
                              ~_T_LBRACE + functionBody + ~_T_RBRACE;
        classDeclaration = _K_CLASS + _ID + classTail;
        classTail = ~_T_LBRACE + classElements + ~_T_RBRACE;
        classElements = classElement + *(~_T_COMMA + classElements);
        classElement = methodDefinition
                       | emptyStatement
                       | *_T_SHARP + propertyName + _T_ASSIGN + singleExpression;
        methodDefinition = *_T_SHARP + propertyName + ~_T_LPARAN + *formalParameterList + ~_T_RPARAN +
                           ~_T_LBRACE + functionBody + ~_T_RBRACE
                           | getter + ~_T_LPARAN + ~_T_RPARAN +
                             ~_T_LBRACE + functionBody + ~_T_RBRACE
                           | setter + ~_T_LPARAN + *formalParameterArg + ~_T_RPARAN +
                             ~_T_LBRACE + functionBody + ~_T_RBRACE;
        formalParameterList = assignable + *(~_T_COMMA + formalParameterList);
        formalParameterArg = assignable;
        functionBody = *functionBody + sourceElements;
        sourceElements = *sourceElements + statement;
        sourceElement = statement;
        statement = block
                    | variableStatement
                    | emptyStatement
                    | classDeclaration
                    | expressionStatement
                    | ifStatement
                    | iterationStatement
                    | continueStatement
                    | breakStatement
                    | returnStatement
                    | withStatement
                    | labelledStatement
                    | switchStatement
                    | throwStatement
                    | tryStatement
                    | debuggerStatement
                    | functionDeclaration;
        block = ~_T_LBRACE + *statementList + ~_T_RBRACE;
        statementList = *statementList + statement;
        expressionStatement = expressionSequence + *eos;
        expressionSequence = singleExpression + *(~_T_COMMA + singleExpression);
        thisExpression = _K_THIS;
        identifierExpression = _ID;
        superExpression = _K_SUPER;
        literalExpression = literal;
        arrayLiteralExpression = arrayLiteral;
        objectLiteralExpression = objectLiteral;
        parenthesizedExpression = ~_T_LPARAN + expressionSequence + ~_T_RPARAN;
        functionExpression = _K_CLASS + ~_ID + classTail
                             | anonymousFunction
                             | thisExpression
                             | identifierExpression
                             | superExpression
                             | literalExpression
                             | arrayLiteralExpression
                             | objectLiteralExpression
                             | parenthesizedExpression;
        classExpression = *(_K_CLASS + ~_ID + classTail) + functionExpression;
        memberIndexExpression = *(memberIndexExpression + _T_LSQUARE + expressionSequence + _T_RSQUARE) +
                                classExpression;
        memberDotExpression = *(memberDotExpression + *_T_QUERY + _T_DOT + *_T_SHARP + identifierName) +
                              memberIndexExpression;
        argumentsExpression = *(argumentsExpression + arguments) + memberDotExpression;
        newExpression = *(_K_NEW + singleExpression + *arguments) + argumentsExpression;
        postIncrementExpression = *(postIncrementExpression + _T_INC) + newExpression;
        postDecreaseExpression = postIncrementExpression + _T_DEC | postIncrementExpression;
        deleteExpression = *(_K_DELETE + deleteExpression) + postDecreaseExpression;
        voidExpression = *(_K_VOID + voidExpression) + deleteExpression;
        typeofExpression = *(_K_TYPEOF + typeofExpression) + voidExpression;
        preIncrementExpression = *(_T_INC + preIncrementExpression) + typeofExpression;
        preDecreaseExpression = *(_T_DEC + preDecreaseExpression) + preIncrementExpression;
        unaryPlusExpression = *(_T_ADD + unaryPlusExpression) + preDecreaseExpression;
        unaryMinusExpression = *(_T_SUB + unaryMinusExpression) + unaryPlusExpression;
        bitNotExpression = *(_T_BIT_NOT + bitNotExpression) + unaryMinusExpression;
        notExpression = *(_T_LOG_NOT + notExpression) + bitNotExpression;
        powerExpression = *(powerExpression + _T_POWER) + notExpression;
        multiplicativeExpression = *(multiplicativeExpression + (_T_MUL | _T_DIV | _T_MOD)) + powerExpression;
        additiveExpression = *(additiveExpression + (_T_ADD | _T_SUB)) + multiplicativeExpression;
        coalesceExpression = *(coalesceExpression + (_T_COALESCE)) + additiveExpression;
        bitShiftExpression = *(bitShiftExpression + (_T_LSHIFT | _T_RSHIFT | _T_URSHIFT)) + coalesceExpression;
        relationalExpression = *(relationalExpression + (_T_LESS | _T_LESS_EQUAL | _T_GREATER | _T_GREATER_EQUAL)) +
                               bitShiftExpression;
        instanceofExpression = *(instanceofExpression + _K_INSTANCEOF) + relationalExpression;
        inExpression = *(inExpression + _K_IN) + instanceofExpression;
        equalityExpression = *(equalityExpression + (_T_EQUAL | _T_NOT_EQUAL | _T_FEQUAL | _T_FNOT_EQUAL)) +
                             inExpression;
        bitAndExpression = *(bitAndExpression + _T_BIT_AND) + equalityExpression;
        bitXOrExpression = *(bitXOrExpression + _T_BIT_XOR) + bitAndExpression;
        bitOrExpression = *(bitOrExpression + _T_BIT_OR) + bitXOrExpression;
        logicalAndExpression = *(logicalAndExpression + _T_LOG_AND) + bitOrExpression;
        logicalOrExpression = *(logicalOrExpression + _T_LOG_OR) + logicalAndExpression;
        ternaryExpression = logicalOrExpression + *(_T_QUERY + singleExpression + _T_COLON + singleExpression);
        assignmentOperatorExpression = *(assignmentOperatorExpression +
                                         (_T_ASSIGN_ADD | _T_ASSIGN_SUB | _T_ASSIGN_MUL | _T_ASSIGN_DIV |
                                          _T_ASSIGN_MOD | _T_ASSIGN_LSHIFT | _T_ASSIGN_RSHIFT | _T_ASSIGN_URSHIFT |
                                          _T_ASSIGN_AND | _T_ASSIGN_OR | _T_ASSIGN_XOR | _T_ASSIGN_POWER)) +
                                       ternaryExpression;
        assignmentExpression = *(assignmentExpression + _T_ASSIGN) + assignmentOperatorExpression;
        singleExpression = *(singleExpression + ~_T_COMMA) + assignmentExpression;
        literal = _K_NULL | _K_TRUE | _K_FALSE | _STRING | _REGEX | _NUMBER;
        arrayLiteral = ~_T_LSQUARE + elementList + ~_T_RSQUARE;
        elementList = *(elementList + *~_T_COMMA) + arrayElement;
        arrayElement = *_T_ELLIPSIS + singleExpression;
        objectLiteral = ~_T_LBRACE + propertyAssignments + *~_T_COMMA + ~_T_RBRACE;
        identifierName = _ID;
        numericLiteral = _NUMBER;
        assignable = _ID | arrayLiteral | objectLiteral;
        arguments = ~_T_LPARAN + *argument + ~_T_RPARAN;
        argument = *(argument + ~_T_COMMA) + *~_T_ELLIPSIS + (singleExpression | _ID);
        propertyAssignments = *(propertyAssignments + ~_T_COMMA) + propertyAssignment;
        propertyAssignment = propertyExpressionAssignment
                             | computedPropertyExpressionAssignment
                             | propertyGetter
                             | propertySetter
                             | propertyShorthand;
        propertyExpressionAssignment = propertyName + ~_T_ASSIGN + singleExpression;
        computedPropertyExpressionAssignment = ~_T_LPARAN + singleExpression + ~_T_RPARAN +
                                               _T_ASSIGN + singleExpression;
        propertyGetter = getter + ~_T_LPARAN + ~_T_RPARAN + ~_T_LBRACE + functionBody + ~_T_RBRACE;
        propertySetter = setter + ~_T_LPARAN + formalParameterArg + ~_T_RPARAN + ~_T_LBRACE + functionBody + ~_T_RBRACE;
        propertyShorthand = *_T_ELLIPSIS + singleExpression;
        propertyName = identifierName
                       | _STRING
                       | numericLiteral
                       | ~_T_LSQUARE + singleExpression + ~_T_RSQUARE;
        anonymousFunction = functionDecl
                            | anoymousFunctionDecl
                            | arrowFunction;
        functionDecl = functionDeclaration;
        anoymousFunctionDecl = _K_FUNCTION + ~_T_LPARAN + *formalParameterList + ~_T_RPARAN +
                               ~_T_LBRACE + functionBody + ~_T_RBRACE;
        arrowFunction = arrowFunctionParameters + ~_T_ARROW + arrowFunctionBody;
        arrowFunctionParameters = _ID | ~_T_LPARAN + *formalParameterList + ~_T_RPARAN;
        arrowFunctionBody = singleExpression | _T_LBRACE + functionBody + _T_RBRACE;
        getter = _K_GET + propertyName;
        setter = _K_SET + propertyName;
        eos = _T_SEMI | _END;
        unit.gen(&program);
#if DUMP_PDA
        std::ofstream of(DUMP_PDA_FILE);
        unit.dump(of);
#endif
    }

    void check_ast(ast_node *node) {
#if CHECK_AST
        if (node->child) {
            auto &c = node->child;
            auto i = c;
            assert(i->parent == node);
            check_ast(i);
            if (i->next != i) {
                assert(i->prev->next == i);
                assert(i->next->prev == i);
                i = i->next;
                do {
                    assert(i->parent == node);
                    assert(i->prev->next == i);
                    assert(i->next->prev == i);
                    check_ast(i);
                    i = i->next;
                } while (i != c);
            } else {
                assert(i->prev == i);
            }
        }
#endif
    }

    void cjsparser::program() {
#if REPORT_ERROR
        std::ofstream log(REPORT_ERROR_FILE, std::ios::app | std::ios::out);
#endif
        next();
        state_stack.clear();
        ast_stack.clear();
        ast_cache.clear();
        ast_cache_index = 0;
        ast_coll_cache.clear();
        ast_reduce_cache.clear();
        state_stack.push_back(0);
        const auto &pdas = unit.get_pda();
        auto root = ast->new_node(a_collection);
        root->line = root->column = 0;
        root->data._coll = pdas[0].coll;
        cjsast::set_child(ast->get_root(), root);
        ast_stack.push_back(root);
        std::vector<int> jumps;
        std::vector<int> trans_ids;
        backtrace_t bk_tmp;
        bk_tmp.lexer_index = 0;
        bk_tmp.state_stack = state_stack;
        bk_tmp.ast_stack = ast_stack;
        bk_tmp.current_state = 0;
        bk_tmp.coll_index = 0;
        bk_tmp.reduce_index = 0;
        bk_tmp.direction = b_next;
        std::vector<backtrace_t> bks;
        bks.push_back(bk_tmp);
        auto trans_id = -1;
        auto prev_idx = 0;
        auto jump_state = -1;
        while (!bks.empty()) {
            auto bk = &bks.back();
            if (bk->direction == b_success || bk->direction == b_fail) {
                break;
            }
            if (bk->direction == b_fallback) {
                if (bk->trans_ids.empty()) {
                    if (bks.size() > 1) {
                        bks.pop_back();
                        bks.back().direction = b_error;
                        bk = &bks.back();
                        if (bk->lexer_index < prev_idx) {
                            bk->direction = b_fail;
                            continue;
                        }
                    } else {
                        bk->direction = b_fail;
                        continue;
                    }
                }
            }
            ast_cache_index = (size_t) bk->lexer_index;
            state_stack = bk->state_stack;
            ast_stack = bk->ast_stack;
            auto state = bk->current_state;
            if (bk->direction != b_error)
                for (;;) {
                    auto is_end = current->t == END && ast_cache_index >= ast_cache.size();
                    const auto &current_state = pdas[state];
                    if (is_end) {
                        if (current_state.final) {
                            if (state_stack.empty()) {
                                bk->direction = b_success;
                                break;
                            }
                        }
                    }
                    auto &trans = current_state.trans;
                    if (trans_id == -1 && !bk->trans_ids.empty()) {
                        trans_id = bk->trans_ids.back() & ((1 << 16) - 1);
                        bk->trans_ids.pop_back();
                    } else {
                        trans_ids.clear();
                        if (is_end) {
                            for (size_t i = 0; i < trans.size(); ++i) {
                                auto &cs = trans[i];
                                if (valid_trans(cs) && (cs.type != e_move && cs.type != e_pass)) {
                                    trans_ids.push_back(i | pda_edge_priority(cs.type) << 16);
                                }
                            }
                        } else {
                            for (size_t i = 0; i < trans.size(); ++i) {
                                auto &cs = trans[i];
                                if (valid_trans(cs)) {
                                    trans_ids.push_back(i | pda_edge_priority(cs.type) << 16);
                                }
                            }
                            if (trans.size() == 1 && !trans_ids.empty() &&
                                (trans[0].type == e_move || trans[0].type == e_pass) &&
                                trans[0].marked) {
                                prev_idx = bk->lexer_index;
                            }
                        }
                        if (!trans_ids.empty()) {
                            std::sort(trans_ids.begin(), trans_ids.end(), std::greater<>());
                            if (trans_ids.size() > 1) {
                                bk_tmp.lexer_index = ast_cache_index;
                                bk_tmp.state_stack = state_stack;
                                bk_tmp.ast_stack = ast_stack;
                                bk_tmp.current_state = state;
                                bk_tmp.trans_ids = trans_ids;
                                bk_tmp.coll_index = ast_coll_cache.size();
                                bk_tmp.reduce_index = ast_reduce_cache.size();
                                bk_tmp.direction = b_next;
#if DEBUG_AST
                                for (auto i = 0; i < bks.size(); ++i) {
                                    auto &_bk = bks[i];
                                    fprintf(stdout,
                                            "[DEBUG] Branch old: i=%d, LI=%d, SS=%d, AS=%d, S=%d, TS=%d, CI=%d, RI=%d, TK=%d\n",
                                            i, _bk.lexer_index, _bk.state_stack.size(),
                                            _bk.ast_stack.size(), _bk.current_state, _bk.trans_ids.size(),
                                            _bk.coll_index, _bk.reduce_index, _bk.ast_ids.size());
                                }
#endif
                                bks.push_back(bk_tmp);
                                bk = &bks.back();
#if DEBUG_AST
                                fprintf(stdout,
                                        "[DEBUG] Branch new: BS=%d, LI=%d, SS=%d, AS=%d, S=%d, TS=%d, CI=%d, RI=%d, TK=%d\n",
                                        bks.size(), bk_tmp.lexer_index, bk_tmp.state_stack.size(),
                                        bk_tmp.ast_stack.size(), bk_tmp.current_state, bk_tmp.trans_ids.size(),
                                        bk_tmp.coll_index, bk_tmp.reduce_index, bk_tmp.ast_ids.size());
#endif
                                bk->direction = b_next;
                                break;
                            } else {
                                trans_id = trans_ids.back() & ((1 << 16) - 1);
                                trans_ids.pop_back();
                            }
                        } else {
#if TRACE_PARSING
                            std::cout << "parsing error: " << current_state.label << std::endl;
#endif
#if REPORT_ERROR
                            log << "parsing error: " << current_state.label << std::endl;
#endif
                            bk->direction = b_error;
                            /*if (semantic) {
                                semantic->error_handler(state, current_state.trans, jump_state);
                            }*/
                            break;
                        }
                    }
                    auto &t = trans[trans_id];
                    if (t.type == e_finish) {
                        if (!is_end) {
#if TRACE_PARSING
                            std::cout << "parsing redundant code: " << current_state.label << std::endl;
#endif
#if REPORT_ERROR
                            log << "parsing redundant code: " << current_state.label << std::endl;
#endif
                            bk->direction = b_error;
                            break;
                        }
                    }
                    auto jump = trans[trans_id].jump;
#if TRACE_PARSING
                    fprintf(stdout, "[%d:%d:%d]%s State: %3d => To: %3d   -- Action: %-10s -- Rule: %s\n",
                            ast_cache_index, ast_stack.size(), bks.size(), is_end ? "*" : "", state, jump,
                            pda_edge_str(t.type).c_str(), current_state.label.c_str());

#endif
#if REPORT_ERROR
                    {
                        static char fmt[256];
                        int line = 0, column = 0;
                        if (ast_cache_index < ast_cache.size()) {
                            line = ast_cache[ast_cache_index]->line;
                            column = ast_cache[ast_cache_index]->column;
                        } else {
                            line = current->line;
                            column = current->column;
                        }
                        snprintf(fmt, sizeof(fmt),
                                 "[%d,%d:%d:%d:%d]%s State: %3d => To: %3d   -- Action: %-10s -- Rule: %s",
                                 line, column, ast_cache_index, ast_stack.size(),
                                 bks.size(), is_end ? "*" : "", state, jump,
                                 pda_edge_str(t.type).c_str(), current_state.label.c_str());
                        log << fmt << std::endl;
                    }
#endif
                    do_trans(state, *bk, trans[trans_id]);
                    auto old_state = state;
                    state = jump;
                    if (semantic) {
                        // DETERMINE LR JUMP BEFORE PARSING AST
                        bk->direction = semantic->check(trans[trans_id].type, ast_stack.back());
                        if (bk->direction == b_error) {
#if TRACE_PARSING
                            std::cout << "parsing semantic error: " << current_state.label << std::endl;
#endif
#if REPORT_ERROR
                            log << "parsing semantic error: " << current_state.label << std::endl;
#endif
                            break;
                        }
                    }
                }
            if (bk->direction == b_error) {
#if DEBUG_AST
                for (auto i = 0; i < bks.size(); ++i) {
                    auto &_bk = bks[i];
                    fprintf(stdout,
                            "[DEBUG] Backtrace failed: i=%d, LI=%d, SS=%d, AS=%d, S=%d, TS=%d, CI=%d, RI=%d, TK=%d\n",
                            i, _bk.lexer_index, _bk.state_stack.size(),
                            _bk.ast_stack.size(), _bk.current_state, _bk.trans_ids.size(),
                            _bk.coll_index, _bk.reduce_index, _bk.ast_ids.size());
                }
#endif
                for (auto &i : bk->ast_ids) {
                    auto &token = ast_cache[i];
                    check_ast(token);
#if DEBUG_AST
                    fprintf(stdout, "[DEBUG] Backtrace failed, unlink token: %p, PB=%p\n", token, token->parent);
#endif
                    cjsast::unlink(token);
                    check_ast(token);
                }
                auto size = ast_reduce_cache.size();
                for (auto i = size; i > bk->reduce_index; --i) {
                    auto &coll = ast_reduce_cache[i - 1];
                    check_ast(coll);
#if DEBUG_AST
                    fprintf(stdout, "[DEBUG] Backtrace failed, unlink: %p, PB=%p, NE=%d, CB=%d\n",
                            coll, coll->parent, cjsast::children_size(coll->parent), cjsast::children_size(coll));
#endif
                    cjsast::unlink(coll);
                    check_ast(coll);
                }
                ast_reduce_cache.erase(ast_reduce_cache.begin() + bk->reduce_index, ast_reduce_cache.end());
                size = ast_coll_cache.size();
                for (auto i = size; i > bk->coll_index; --i) {
                    auto &coll = ast_coll_cache[i - 1];
                    assert(coll->flag == a_collection);
                    check_ast(coll);
#if DEBUG_AST
                    fprintf(stdout, "[DEBUG] Backtrace failed, delete coll: %p, PB=%p, CB=%p, NE=%d, CS=%d\n",
                            coll, coll->parent, coll->child,
                            cjsast::children_size(coll->parent), cjsast::children_size(coll));
#endif
                    cjsast::unlink(coll);
                    check_ast(coll);
                    ast->remove(coll);
                }
                ast_coll_cache.erase(ast_coll_cache.begin() + bk->coll_index, ast_coll_cache.end());
                bk->direction = b_fallback;
            }
            trans_id = -1;
        }
    }

    ast_node *cjsparser::terminal() {
        if (current->t == END && ast_cache_index >= ast_cache.size()) { // 结尾
            error("unexpected token EOF of expression");
        }
        if (ast_cache_index < ast_cache.size()) {
            return ast_cache[ast_cache_index++];
        }
        if (current->t > OPERATOR_START && current->t < OPERATOR_END) {
            auto node = ast->new_node(a_operator);
            node->line = current->line;
            node->column = current->column;
            node->start = current->start;
            node->end = current->end;
            node->data._op = current->t;
            match_type(node->data._op);
            ast_cache.push_back(node);
            ast_cache_index++;
            return node;
        }
        if (current->t > KEYWORD_START && current->t < KEYWORD_END) {
            auto node = ast->new_node(a_keyword);
            node->line = current->line;
            node->column = current->column;
            node->start = current->start;
            node->end = current->end;
            node->data._keyword = current->t;
            match_type(node->data._keyword);
            ast_cache.push_back(node);
            ast_cache_index++;
            return node;
        }
        if (current->t == ID) {
            auto node = ast->new_node(a_literal);
            node->line = current->line;
            node->column = current->column;
            node->start = current->start;
            node->end = current->end;
            ast->set_str(node, lexer->get_data(current->idx));
            match_type(current->t);
            ast_cache.push_back(node);
            ast_cache_index++;
            return node;
        }
        if (current->t == NUMBER) {
            auto node = ast->new_node(a_number);
            node->line = current->line;
            node->column = current->column;
            node->start = current->start;
            node->end = current->end;
            node->data._number = *(double *) lexer->get_data(current->idx);
            match_type(current->t);
            ast_cache.push_back(node);
            ast_cache_index++;
            return node;
        }
        if (current->t == STRING) {
            std::stringstream ss;
            ss << lexer->get_data(current->idx);
            match_type(current->t);

            while (current->t == STRING) {
                ss << lexer->get_data(current->idx);
                match_type(current->t);
            }
            auto node = ast->new_node(a_string);
            node->line = current->line;
            node->column = current->column;
            node->start = current->start;
            node->end = current->end;
            ast->set_str(node, ss.str());
            ast_cache.push_back(node);
            ast_cache_index++;
            return node;
        }
        if (current->t == REGEX) {
            auto node = ast->new_node(a_regex);
            node->line = current->line;
            node->column = current->column;
            node->start = current->start;
            node->end = current->end;
            ast->set_str(node, lexer->get_data(current->idx));
            match_type(current->t);
            ast_cache.push_back(node);
            ast_cache_index++;
            return node;
        }
        error("invalid type");
        return nullptr;
    }

    bool cjsparser::valid_trans(const pda_trans &trans) const {
        auto &la = trans.LA;
        if (!la.empty()) {
            auto success = false;
            for (auto &_la : la) {
                if (LA(_la)) {
                    success = true;
                    break;
                }
            }
            if (!success)
                return false;
        }
        switch (trans.type) {
            case e_reduce:
            case e_reduce_exp: {
                if (ast_stack.size() <= 1)
                    return false;
                if (state_stack.empty())
                    return false;
                if (trans.status != state_stack.back())
                    return false;
            }
                break;
            default:
                break;
        }
        return true;
    }

    void cjsparser::do_trans(int state, backtrace_t &bk, const pda_trans &trans) {
        switch (trans.type) {
            case e_shift: {
                state_stack.push_back(state);
                auto new_node = ast->new_node(a_collection);
                new_node->line = new_node->column = 0;
                auto &pdas = unit.get_pda();
                new_node->data._coll = pdas[trans.jump].coll;
#if DEBUG_AST
                fprintf(stdout, "[DEBUG] Shift: top=%p, new=%p, CS=%d\n", ast_stack.back(), new_node,
                        cjsast::children_size(ast_stack.back()));
#endif
                ast_coll_cache.push_back(new_node);
                ast_stack.push_back(new_node);
            }
                break;
            case e_pass: {
                bk.ast_ids.insert(ast_cache_index);
                auto t = terminal();
#if CHECK_AST
                check_ast(t);
#endif
#if DEBUG_AST
                fprintf(stdout, "[DEBUG] Move: parent=%p, child=%p, CS=%d\n", ast_stack.back(), t,
                        cjsast::children_size(ast_stack.back()));
#endif
            }
                break;
            case e_move: {
                bk.ast_ids.insert(ast_cache_index);
                auto t = terminal();
#if CHECK_AST
                check_ast(t);
#endif
#if DEBUG_AST
                fprintf(stdout, "[DEBUG] Move: parent=%p, child=%p, CS=%d\n", ast_stack.back(), t,
                        cjsast::children_size(ast_stack.back()));
#endif
                cjsast::set_child(ast_stack.back(), t);
            }
                break;
            case e_left_recursion:
                break;
            case e_left_recursion_not_greed:
                break;
            case e_reduce:
            case e_reduce_exp: {
                auto new_ast = ast_stack.back();
                check_ast(new_ast);
                if (new_ast->flag != a_collection) {
                    bk.ast_ids.insert(ast_cache_index);
                }
                state_stack.pop_back();
                ast_stack.pop_back();
                assert(!ast_stack.empty());
                ast_reduce_cache.push_back(new_ast);
#if DEBUG_AST
                fprintf(stdout, "[DEBUG] Reduce: parent=%p, child=%p, CS=%d, AS=%d, RI=%d\n",
                        ast_stack.back(), new_ast, cjsast::children_size(ast_stack.back()),
                        ast_stack.size(), ast_reduce_cache.size());
#endif
                if (trans.type == e_reduce_exp)
                    ast_stack.back()->attr |= a_exp;
                cjsast::set_child(ast_stack.back(), new_ast);
                check_ast(ast_stack.back());
            }
                break;
            case e_finish:
                state_stack.pop_back();
                break;
        }
    }

    bool cjsparser::LA(struct unit *u) const {
        if (u->t != u_token)
            return false;
        auto token = to_token(u);
        if (ast_cache_index < ast_cache.size()) {
            auto &cache = ast_cache[ast_cache_index];
            if (token->type > KEYWORD_START && token->type < KEYWORD_END)
                return cache->flag == a_keyword && cache->data._keyword == token->type;
            if (token->type > OPERATOR_START && token->type < OPERATOR_END)
                return cache->flag == a_operator && cache->data._op == token->type;
            return cjsast::ast_equal((ast_t) cache->flag, token->type);
        }
        return current->t == token->type;
    }

    void cjsparser::expect(bool flag, const std::string &info) {
        if (!flag) {
            error(info);
        }
    }

    void cjsparser::match_type(lexer_t type) {
        expect(current->t == type, std::string("expect get_type ") + lexer_string(type));
        next();
    }

    void cjsparser::error(const std::string &info) {
        std::stringstream ss;
        ss << '[' << std::setfill('0') << std::setw(4) << current->line;
        ss << ':' << std::setfill('0') << std::setw(3) << current->column;
        ss << ']' << ' ' << info;
        throw cexception(ss.str());
    }
}