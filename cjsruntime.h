//
// Project: clibjs
// Created by bajdcc
//

#ifndef CLIBJS_CJSRUNTIME_H
#define CLIBJS_CJSRUNTIME_H

#include <list>
#include "cjsgen.h"

namespace clib {

    class jsv_number;

    class jsv_string;

    class jsv_boolean;

    class jsv_regex;

    class jsv_array;

    class jsv_object;

    class jsv_function;

    class js_value_new {
    public:
        virtual std::shared_ptr<jsv_number> new_number(double n) = 0;
        virtual std::shared_ptr<jsv_string> new_string(const std::string &s) = 0;
        virtual std::shared_ptr<jsv_boolean> new_boolean(bool b) = 0;
        virtual std::shared_ptr<jsv_regex> new_regex(const std::string &s) = 0;
        virtual std::shared_ptr<jsv_array> new_array() = 0;
        virtual std::shared_ptr<jsv_object> new_object() = 0;
        virtual std::shared_ptr<jsv_function> new_function() = 0;
    };

    class js_value : public std::enable_shared_from_this<js_value> {
    public:
        using ref = std::shared_ptr<js_value>;
        using weak_ref = std::weak_ptr<js_value>;
        virtual js_value::ref clone() const = 0;
        virtual runtime_t get_type() = 0;
        virtual js_value::ref binary_op(js_value_new &n, int code, js_value::ref op) = 0;
        virtual bool to_bool() const = 0;
        virtual void mark(int n) = 0;
        virtual void print(std::ostream &os) = 0;
        int marked{0};
    };

    class jsv_number : public js_value {
    public:
        using ref = std::shared_ptr<jsv_number>;
        using weak_ref = std::weak_ptr<jsv_number>;
        explicit jsv_number(double n);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, js_value::ref op) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        double number;
    };

    class jsv_string : public js_value {
    public:
        using ref = std::shared_ptr<jsv_string>;
        using weak_ref = std::weak_ptr<jsv_string>;
        explicit jsv_string(std::string s);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, js_value::ref op) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        std::string str;
    };

    class jsv_boolean : public js_value {
    public:
        using ref = std::shared_ptr<jsv_boolean>;
        using weak_ref = std::weak_ptr<jsv_boolean>;
        explicit jsv_boolean(bool flag);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, js_value::ref op) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        bool b{false};
    };

    class jsv_regex : public js_value {
    public:
        using ref = std::shared_ptr<jsv_regex>;
        using weak_ref = std::weak_ptr<jsv_regex>;
        explicit jsv_regex(std::string s);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, js_value::ref op) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        ref clear();
        std::string re;
    };

    class jsv_array : public js_value {
    public:
        using ref = std::shared_ptr<jsv_array>;
        using weak_ref = std::weak_ptr<jsv_array>;
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, js_value::ref op) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        ref clear();
        std::vector<js_value::weak_ref> arr;
    };

    class jsv_object : public js_value {
    public:
        using ref = std::shared_ptr<jsv_object>;
        using weak_ref = std::weak_ptr<jsv_object>;
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, js_value::ref op) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        ref clear();
        std::unordered_map<std::string, js_value::weak_ref> obj;
    };

    class cjs_function_info;

    class jsv_function : public js_value {
    public:
        using ref = std::shared_ptr<jsv_function>;
        using weak_ref = std::weak_ptr<jsv_function>;
        jsv_function() = default;
        explicit jsv_function(sym_code_t::ref c);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, js_value::ref op) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        ref clear();
        std::shared_ptr<cjs_function_info> code;
        jsv_object::weak_ref closure;
        std::string name;
    };

    class cjs_function_info : public std::enable_shared_from_this<cjs_function_info> {
    public:
        using ref = std::shared_ptr<cjs_function_info>;
        using weak_ref = std::weak_ptr<cjs_function_info>;
        explicit cjs_function_info(const sym_code_t::ref &code);
        static js_value::ref load_const(const cjs_consts &c, int op);
        std::string fullname;
        std::string text;
        std::vector<std::string> args;
        std::vector<std::string> names;
        std::vector<std::string> globals;
        std::vector<std::string> derefs;
        std::vector<js_value::ref> consts;
        std::vector<cjs_code> codes;
        std::vector<std::string> closure;
    };

    class cjs_function : public std::enable_shared_from_this<cjs_function> {
    public:
        using ref = std::shared_ptr<cjs_function>;
        using weak_ref = std::weak_ptr<cjs_function>;
        explicit cjs_function(sym_code_t::ref code);
        explicit cjs_function(cjs_function_info::ref code);
        void store_name(const std::string &name, js_value::weak_ref obj);
        void store_fast(const std::string &name, js_value::weak_ref obj);
        cjs_function_info::ref info;
        std::string name{"UNKNOWN"};
        int pc{0};
        std::vector<js_value::weak_ref> stack;
        js_value::weak_ref ret_value;
        std::unordered_map<std::string, js_value::weak_ref> envs;
        std::unordered_map<std::string, js_value::weak_ref> closure;
    };

    struct cjs_runtime_reuse {
        std::vector<jsv_number::ref> reuse_numbers;
        std::vector<jsv_string::ref> reuse_strings;
        std::vector<jsv_boolean::ref> reuse_booleans;
        std::vector<jsv_regex::ref> reuse_regexes;
        std::vector<jsv_array::ref> reuse_arrays;
        std::vector<jsv_object::ref> reuse_objects;
        std::vector<jsv_function::ref> reuse_functions;
    };

    class cjsruntime : public js_value_new {
    public:
        cjsruntime();
        ~cjsruntime() = default;

        cjsruntime(const cjsruntime &) = delete;
        cjsruntime &operator=(const cjsruntime &) = delete;

        void eval(cjs_code_result::ref code);

        jsv_number::ref new_number(double n) override;
        jsv_string::ref new_string(const std::string &s) override;
        jsv_boolean::ref new_boolean(bool b) override;
        jsv_regex::ref new_regex(const std::string &s) override;
        jsv_array::ref new_array() override;
        jsv_object::ref new_object() override;
        jsv_function::ref new_function() override;

    private:
        int run(const sym_code_t::ref &fun, const cjs_code &code);
        js_value::ref load_const(int op);
        js_value::ref load_fast(int op);
        js_value::ref load_name(int op);
        js_value::ref load_global(int op);
        js_value::ref load_closure(const std::string &name);
        js_value::ref load_deref(const std::string &name);
        void push(js_value::weak_ref value);
        const js_value::weak_ref &top() const;
        js_value::weak_ref pop();

        js_value::ref register_value(const js_value::ref &value);
        void dump_step(const cjs_code &code) const;
        void dump_step2(const cjs_code &code) const;
        void dump_step3() const;

        void reuse_value(const js_value::ref &);

        void gc();

        static void print(const js_value::ref &value, int level, std::ostream &os);

    private:
        std::vector<cjs_function::ref> stack;
        cjs_function::ref current_stack;
        std::list<js_value::ref> objs;
        struct _permanents_t {
            js_value::ref _null;
            jsv_boolean::ref _true;
            jsv_boolean::ref _false;
        } permanents;
        cjs_runtime_reuse reuse;
    };
}

#endif //CLIBJS_CJSRUNTIME_H
