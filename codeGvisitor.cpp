#include "codeGvisitor.hpp"
#include "output.hpp"
#include <memory>
#include <cassert>
using namespace std;

using namespace ast;

codeGvisitor::codeGvisitor(output::CodeBuffer* cb): cb(cb)
{
 cb->emit(R"(
declare i32 @scanf(i8*, ...)
declare i32 @printf(i8*, ...)
declare void @exit(i32)
@.int_specifier_scan = constant [3 x i8] c"%d\00"
@.int_specifier = constant [4 x i8] c"%d\0A\00"
@.str_specifier = constant [4 x i8] c"%s\0A\00"

define i32 @readi(i32) {
    %ret_val = alloca i32
    %spec_ptr = getelementptr [3 x i8], [3 x i8]* @.int_specifier_scan, i32 0, i32 0
    call i32 (i8*, ...) @scanf(i8* %spec_ptr, i32* %ret_val)
    %val = load i32, i32* %ret_val
    ret i32 %val
}

define void @printi(i32) {
    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0
    call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)
    ret void
}

define void @print(i8*) {
    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0
    call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)
    ret void
}
    )");
}

    void codeGvisitor::visit(ast::ArrayType &node){}//today 
    void codeGvisitor::visit(FuncDecl& node){}//today 
    void codeGvisitor::visit(VarDecl& node) {}//today
    void codeGvisitor::visit(Return& node) {}
    void codeGvisitor::visit(Assign& node) {}//today 
    void codeGvisitor::visit(If& node) {}
    void codeGvisitor::visit(While& node) {}
    void codeGvisitor::visit(Break& node) {}
    void codeGvisitor::visit(Continue& node) {}
    void codeGvisitor::visit(Call& node) {}
    void codeGvisitor::visit(ExpList& node) {}
    void codeGvisitor::visit(ast::ArrayAssign &node) {
node.id->accept(*this);
    node.index->accept(*this);
    node.exp->accept(*this);// accepting everything 

 std::string indexVar = node.index->newVar;
  if (node.index->type == ast::BuiltInType::BYTE) {
        std::string z = cb->freshVar();
        cb->emit(z + " = zext i8 " + indexVar + " to i32");
        indexVar = z;
    }
    std::string expVar = node.exp->newVar;
     int baseoff = node.id->offset;
     std::string newbasePtrI32 = cb->freshVar();
        cb->emit(newbasePtrI32 +
        " = getelementptr i32, i32* %local_vars, i32 " +
        std::to_string(baseoff));
   
    auto arrType = node.id->type;
   

    std::string elemLLVM = output::changeType(arrType);

    std::string basePtr = cb->freshVar();
    cb->emit(basePtr +
        " = bitcast i32* " + newbasePtrI32 + " to " + elemLLVM + "*");

   
    std::string elemPtr = cb->freshVar();
    cb->emit(elemPtr +
        " = getelementptr " + elemLLVM + ", " + elemLLVM + "* " +
        basePtr + ", i32 " + indexVar);

  
    if (node.exp->type == ast::BuiltInType::BYTE &&
        arrType == ast::BuiltInType::INT) {
        std::string rhsExt = cb->freshVar();
        cb->emit(rhsExt + " = zext i8 " + expVar + " to i32");
        expVar = rhsExt;
    } else if (node.exp->type == ast::BuiltInType::INT &&
               arrType == ast::BuiltInType::BYTE) {
        std::string rhsTrunc = cb->freshVar();
        cb->emit(rhsTrunc + " = trunc i32 " + expVar + " to i8");
        expVar = rhsTrunc;
    }

   
    cb->emit("store " + elemLLVM + " " + expVar + ", " +
                    elemLLVM + "* " + elemPtr + ", align 4");



//     idx  = code for index-expression            ; possibly zext to i32
// rhs  = code for right-hand side expression  ; widen/trunc if needed

// base = getelementptr i32, i32* %local_vars, i32 <arrayOffset>
// elemPtr = bitcast i32* base to <elemTy>*      ; if you keep %local_vars
// elemPtr = getelementptr <elemTy>, <elemTy>* elemPtr, i32 idx   ; ← ①
// store <elemTy> rhs, <elemTy>* elemPtr, align 4                 ; ← ②


    }//today
    void codeGvisitor::visit(ast::Funcs& node)
     {for (std::size_t i = 0; i < node.funcs.size(); ++i) {
    node.funcs[i]->accept(*this);
}}
    void codeGvisitor::visit(ast::PrimitiveType &node){}//done
    void codeGvisitor::visit(ast::Formal &node) {}//today
    void codeGvisitor::visit(ast::Formals &node) { for (size_t i = 0; i < node.formals.size(); ++i) {
        node.formals[i]->accept(*this);
        if (i != node.formals.size() - 1) {
           cb->emit(", ");
        }
    }}



    void codeGvisitor::visit(Statements& node){
    for (size_t i = 0; i < node.statements.size(); ++i) {
        node.statements[i]->accept(*this);
    }
}//maybe not complete- maybe we need to check last statement if empty}
 
    void codeGvisitor::visit(Num& node) {
            node.newVar=std::to_string(node.value);
    }
    void codeGvisitor::visit(NumB& node) {node.newVar=std::to_string(node.value);}
    void codeGvisitor::visit(String& node) {  node.newVar=(node.value);}
    void codeGvisitor::visit(Bool& node) 
    {
         if(node.value==true)
    {node.newVar="true";}
    else{node.newVar="false";}}
    void codeGvisitor::visit(ID& node) 
    {//today
    if (node.offset < 0) {
        node.newVar = "%" + node.value;
        return;
    }

    // Local variables (from local_vars array)
    const int offset = node.offset;
    const std::string ltype =output::changeType(node.type);

    // Compute address of the variable: %addr = getelementptr ...
    std::string inbetweenadrrs = cb->freshVar();
    cb->emit(inbetweenadrrs + " = getelementptr i32, i32* %local_vars, i32 " + std::to_string(offset));

    // Cast the pointer to the correct LLVM type: %cast = bitcast ...
    std::string cpointer = cb->freshVar();
    cb->emit(cpointer + " = bitcast i32* " + inbetweenadrrs + " to " + ltype + "*");

    // Load the value from memory: %val = load ...
    std::string newV = cb->freshVar();
    cb->emit(newV + " = load " + ltype + ", " + ltype + "* " + cpointer + ", align 4");

    // Set the result
    node.newVar = newV;
}
    void codeGvisitor::visit(BinOp& node) {}//today
    void codeGvisitor::visit(RelOp& node) {}//today
    void codeGvisitor::visit(Not& node) {}
    void codeGvisitor::visit(And& node) {}
    void codeGvisitor::visit(Or& node) {}
    void codeGvisitor::visit(ArrayDereference& node) {}//today
    void codeGvisitor::visit(Cast& node) {}//today
   //codeGvisitor::visit(ast::PrimitiveType& node) {}