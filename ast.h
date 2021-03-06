#ifndef AST_H
#define AST_H

#include <iostream>
#include <ciso646>

#include "lexer.h"

namespace rift {

typedef int Symbol;

class Visitor;

namespace ast {

    class Exp {
    public:
        virtual ~Exp() {}
        virtual void accept(Visitor * v) = 0;
        void print(std::ostream & s);
    };


    class Num : public Exp {
    public:
        Num(double value):
            value(value) {}

        void accept(Visitor * v) override;


        double value;
    };


    class Str : public Exp {
    public:
        Str(unsigned index):
            index(index) {}
        void accept(Visitor * v) override;
        std::string const & value() const;

        unsigned index;
    };

    class Var : public Exp  {
    public:
        Var(Symbol symbol):
            symbol(symbol) {}
        void accept(Visitor * v) override;
        std::string const & value() const;

        unsigned symbol;
    };

    class Seq : public Exp {
    public:
        ~Seq() override  {
            for (Exp * e : body)
                delete e;
        }

        void accept(Visitor * v) override;

        std::vector<Exp*> body;
    };

    class Fun: public Exp {
    public:

        Fun():
            body(nullptr) {
        }

        Fun(ast::Exp * body):
            body(dynamic_cast<ast::Seq *>(body)) {
        }

        ~Fun() {
            for (Var * v : args)
                delete v;
            delete body;
        }

        void accept(Visitor * v) override;

        Seq * body;
        std::vector<Var *> args;
    };

    class BinExp : public Exp {
    public:
        enum class Type {
            add,
            sub,
            mul,
            div,
            eq,
            neq,
            lt,
            gt,
        };

        BinExp(Exp * lhs, Exp * rhs, Type t):
            lhs(lhs),
            rhs(rhs),
            type(t) {
        }

        ~BinExp() {
            delete lhs;
            delete rhs;
        }

        void accept(Visitor * v) override;

        Exp * lhs;
        Exp * rhs;
        Type type;
    };

    class Call: public Exp {
    public:

        ~Call() {
            for (Exp * e : args)
                delete e;
        }

        void accept(Visitor * v) override;

        std::vector<Exp*> args;

    };

    class UserCall : public Call {
    public:
        UserCall(Var * name):
            name(name) {
        }
        ~UserCall() {
            delete name;
        }

        void accept(Visitor * v) override;

        Var * name;
    };

    class SpecialCall : public Call {
    public:

        void accept(Visitor * v) override;

    };

    class CCall : public SpecialCall {
    public:
        void accept(Visitor * v) override;
    };

    class EvalCall : public SpecialCall {
    public:
        EvalCall(ast::Exp * arg) {
            args.push_back(arg);
        }

        void accept(Visitor * v) override;
    };

    class LengthCall : public SpecialCall {
    public:
        LengthCall(ast::Exp * arg) {
            args.push_back(arg);
        }
        void accept(Visitor * v) override;
    };

    class TypeCall : public SpecialCall {
    public:
        TypeCall(ast::Exp * arg) {
            args.push_back(arg);
        }
        void accept(Visitor * v) override;
    };


    class Index : public Exp {

    public:
        Index(Var * name):
            name(name),
            index(nullptr) {
        }

        ~Index() {
            delete name;
            delete index;
        }

        void accept(Visitor * v) override;

        Var * name;
        Exp * index;
    };

    class Assignment : public Exp {
    public:
        void accept(Visitor * v) override;

    };

    class SimpleAssignment : public Assignment {
    public:

        SimpleAssignment(Var * name):
            name(name),
            rhs(nullptr) {
        }

        ~SimpleAssignment() {
            delete name;
            delete rhs;
        }

        void accept(Visitor * v) override;

        Var * name;
        Exp * rhs;

    };

    class IndexAssignment : public Assignment {
    public:

        IndexAssignment(Index * index):
            index(index),
            rhs(nullptr) {
        }

        ~IndexAssignment() override {
            delete index;
            delete rhs;
        }

        void accept(Visitor * v) override;

        Index * index;
        Exp * rhs;

    };

    class IfElse : public Exp {
    public:
        IfElse(Exp * guard):
            guard(guard),
            ifClause(nullptr),
            elseClause(nullptr) {
        }

        ~IfElse() {
            delete guard;
            delete ifClause;
            delete elseClause;
        }

        void accept(Visitor * v) override;

        Exp * guard;
        Exp * ifClause;
        Exp * elseClause;
    };

    class WhileLoop : public Exp {
    public:
        WhileLoop(Exp * guard):
            guard(guard),
            body(nullptr) {
        }

        ~WhileLoop() override {
            delete guard;
            delete body;
        }

        void accept(Visitor * v) override;

        Exp * guard;
        Seq * body;
    };




} // namespace ast

class Visitor {
public:
    virtual void visit(ast::Exp * node) {}
    virtual void visit(ast::Num * node) { visit(static_cast<ast::Exp*>(node)); }
    virtual void visit(ast::Str * node) { visit(static_cast<ast::Exp*>(node)); }
    virtual void visit(ast::Var * node) { visit(static_cast<ast::Exp*>(node)); }
    virtual void visit(ast::Seq * node) { visit(static_cast<ast::Exp*>(node)); }
    virtual void visit(ast::Fun * node) { visit(static_cast<ast::Exp*>(node)); }
    virtual void visit(ast::BinExp * node) { visit(static_cast<ast::Exp*>(node)); }
    virtual void visit(ast::Call * node) { visit(static_cast<ast::Exp*>(node)); }
    virtual void visit(ast::UserCall * node) { visit(static_cast<ast::Call*>(node)); }
    virtual void visit(ast::SpecialCall * node) { visit(static_cast<ast::Call*>(node)); }
    virtual void visit(ast::CCall * node) { visit(static_cast<ast::SpecialCall*>(node)); }
    virtual void visit(ast::EvalCall * node) { visit(static_cast<ast::SpecialCall*>(node)); }
    virtual void visit(ast::TypeCall * node) { visit(static_cast<ast::SpecialCall*>(node)); }
    virtual void visit(ast::LengthCall * node) { visit(static_cast<ast::SpecialCall*>(node)); }
    virtual void visit(ast::Index * node) { visit(static_cast<ast::Exp*>(node)); }
    virtual void visit(ast::Assignment * node) { visit(static_cast<ast::Exp*>(node)); }
    virtual void visit(ast::SimpleAssignment * node) { visit(static_cast<ast::Assignment*>(node)); }
    virtual void visit(ast::IndexAssignment * node) { visit(static_cast<ast::Assignment*>(node)); }
    virtual void visit(ast::IfElse * node) { visit(static_cast<ast::Exp*>(node)); }
    virtual void visit(ast::WhileLoop * node) { visit(static_cast<ast::Exp*>(node)); }
};


} // namespace rift2

#endif // AST_H

