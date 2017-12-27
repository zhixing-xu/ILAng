/// file
/// Unit test for ExprConst

#include "ila/ast/expr_const.h"
#include "ila/ast/sort_value.h"
#include "util_test.h"
#include "z3++.h"

namespace ila {

TEST(ExprConst, Construct) {
  EXPECT_DEATH(ExprConst(), ".*");
  auto bool_const = std::make_shared<ExprConst>(BoolVal(true));
  auto bv_const = std::make_shared<ExprConst>(BvVal(1), 8);

  MemValMap mem_map;
  for (int i = 0; i < 10; i++) {
    mem_map[i] = i + 1;
  }
  MemVal mem_val(1, mem_map);
  auto mem_const = std::make_shared<ExprConst>(8, 32);

  EXPECT_TRUE(bool_const->is_const());
  EXPECT_FALSE(bv_const->is_op());
  EXPECT_FALSE(mem_const->is_var());
}

TEST(ExprConst, BoolZ3Expr) {
  z3::context ctx;
  Z3ExprVec arg_vec;
  z3::solver s(ctx);

  auto bool_const_true = std::make_shared<ExprConst>(BoolVal(true));
  auto bool_const_false = std::make_shared<ExprConst>(BoolVal("False"));
  auto e_true = bool_const_true->GetZ3Expr(ctx, arg_vec);
  auto e_false = bool_const_false->GetZ3Expr(ctx, arg_vec);
  auto bool_eq = (e_true == e_false);
  s.add(bool_eq);
  EXPECT_EQ(z3::unsat, s.check());
}

TEST(ExprConst, BvZ3Expr) {
  z3::context ctx;
  Z3ExprVec arg_vec;
  z3::solver s(ctx);

  auto bv_const_0 = std::make_shared<ExprConst>(BvVal("0"), 8);
  auto bv_const_1 = std::make_shared<ExprConst>(BvVal(1), 8);
  auto e_0 = bv_const_0->GetZ3Expr(ctx, arg_vec);
  auto e_1 = bv_const_1->GetZ3Expr(ctx, arg_vec);
  auto bv_eq = ((e_0 + e_1) == e_1);
  s.add(!bv_eq);
  EXPECT_EQ(z3::unsat, s.check());
}

TEST(ExprConst, MemZ3Expr) {
  z3::context ctx;
  Z3ExprVec arg_vec;
  z3::solver s(ctx);

  MemValMap mem_map;
  for (int i = 0; i < 10; i++) {
    mem_map[i] = i + 1;
  }
  MemVal mem_val(1, mem_map);
  auto mem_const_0 = std::make_shared<ExprConst>(mem_val, 8, 32);

  MemVal mem_val_copy(mem_val.def_val());
  for (int i = 0; i < 5; i++) {
    mem_val_copy.set_data(i, mem_val.get_data(i));
  }
  MemValMap mem_map_copy = mem_val.val_map();
  for (int i = 5; i < 10; i++) {
    mem_val_copy.set_data(i, mem_map_copy[i]);
  }
  auto mem_const_1 = std::make_shared<ExprConst>(mem_val_copy, 8, 32);

  auto e_mem_0 = mem_const_0->GetZ3Expr(ctx, arg_vec);
  auto e_mem_1 = mem_const_1->GetZ3Expr(ctx, arg_vec);
  auto mem_eq = (e_mem_0 == e_mem_1);
  s.add(!mem_eq);
  EXPECT_EQ(z3::unsat, s.check());
}

TEST(ExprConst, BoolVal) {
  EXPECT_DEATH(BoolVal(), ".*");
  auto bool_const = std::make_shared<ExprConst>(BoolVal(true));

  auto bool_val = bool_const->val_bool();
  EXPECT_DEATH(bool_const->val_bv(), ".*");
  EXPECT_DEATH(bool_const->val_mem(), ".*");

  EXPECT_EQ(true, bool_val->val());

  std::string ref_str = "TRUE";
  std::string msg;

  EXPECT_EQ(ref_str, bool_val->str());

  GET_STDOUT_MSG(bool_val->Print(std::cout), msg);
  EXPECT_EQ(ref_str, msg);

  GET_STDOUT_MSG(std::cout << *bool_val, msg);
  EXPECT_EQ(ref_str, msg);
}

TEST(ExprConst, BvVal) {
  EXPECT_DEATH(BvVal(), ".*");
  auto bv_const = std::make_shared<ExprConst>(BvVal(1), 8);

  auto bv_val = bv_const->val_bv();
  EXPECT_DEATH(bv_const->val_bool(), ".*");
  EXPECT_DEATH(bv_const->val_mem(), ".*");

  EXPECT_EQ(1, bv_val->val());

  std::string ref_str = "1";
  std::string msg;

  EXPECT_EQ(ref_str, bv_val->str());

  GET_STDOUT_MSG(bv_val->Print(std::cout), msg);
  EXPECT_EQ(ref_str, msg);

  GET_STDOUT_MSG(std::cout << *bv_val, msg);
  EXPECT_EQ(ref_str, msg);
}

TEST(ExprConst, MemVal) {
  EXPECT_DEATH(MemVal(), ".*");
  int def = 1;
  MemVal val(def);
  for (int i = 0; i < 2; i++) {
    val.set_data(i, i * 2);
  }
  auto mem_const = std::make_shared<ExprConst>(val, 8, 32);

  auto mem_val = mem_const->val_mem();
  EXPECT_DEATH(mem_const->val_bool(), ".*");
  EXPECT_DEATH(mem_const->val_bv(), ".*");

  EXPECT_EQ(def, mem_val->def_val());
  MemValMap val_map = mem_val->val_map();
  EXPECT_EQ(0, val_map[0]);
  EXPECT_EQ(2, mem_val->get_data(1));
  EXPECT_EQ(def, mem_val->get_data(2));

  std::string ref_str = "[Def: 1][(0, 0)(1, 2)]";
  std::string msg;

  GET_STDOUT_MSG(mem_val->Print(std::cout), msg);
  EXPECT_EQ(ref_str, msg);

  GET_STDOUT_MSG(std::cout << *mem_val, msg);
  EXPECT_EQ(ref_str, msg);
}

} // namespace ila

