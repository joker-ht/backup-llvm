//===- InstIndex.h - Instruction Index Information ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines a number of light weight data structures used
// to describe the unique identification of an instruction.
//
//===----------------------------------------------------------------------===//
// dingzhu patch

#ifndef LLVM_InstIndex
#define LLVM_InstIndex

// dingzhu patch
#include <set>
#include <string>
#include <tuple>

namespace llvm {

  class raw_ostream;

  class InstIndex {
    // format: funcname, BBlabel, Inst num in bb
    std::tuple<std::string, std::string, int> Index;
    std::string FuncName;
    std::string BBLabel;
    int InstNum;

  public:
    InstIndex() = default;
    
    InstIndex(const InstIndex& SrcIndex);

    InstIndex& operator = (const InstIndex& SrcIndex);

    void setFuncName(std::string SrcFuncName) {
      FuncName = SrcFuncName;
    }

    void setBBLabel(std::string SrcBBLabel) {
      BBLabel = SrcBBLabel;
    }

    void setInstNum(int SrcInstNum) {
      InstNum = SrcInstNum;
    }

    std::string getFuncName() const {
      return FuncName;
    }

    std::string getBBLabel() const {
      return BBLabel;
    }

    int getInstNum() const {
      return InstNum;
    }

    bool operator==(const InstIndex &II) const { return FuncName == II.FuncName && BBLabel == II.BBLabel && InstNum == II.InstNum; }
    bool operator!=(const InstIndex &II) const { return !(FuncName == II.FuncName && BBLabel == II.BBLabel && InstNum == II.InstNum); }

    bool operator<(const InstIndex &II) const {
      if (FuncName != II.FuncName) {
        return FuncName < II.FuncName;
      }
      else if (BBLabel != II.BBLabel){
        return BBLabel < II.BBLabel;
      }
      else {
        return InstNum < II.InstNum;
      }
    }

    void dump() const;

    void print(raw_ostream &OS) const;

  };

typedef std::set<InstIndex> InstIndexSet;


} // end namespace llvm

#endif