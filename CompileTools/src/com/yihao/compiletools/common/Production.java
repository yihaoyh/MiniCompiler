package com.yihao.compiletools.common;

import java.util.ArrayList;

/*
 * 产生式
 */
public class Production {
    /**
     * 产生式左边的非终结符
     */
    public Nonterminal left;

    /**
     * 产生式右边的符号串
     */
    public ArrayList<StringElement>[] right;
}
