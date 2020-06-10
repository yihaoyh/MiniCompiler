package com.yihao.compiletools;

import com.yihao.compiletools.common.Production;

public class Main {

    public static void main(String[] args) {
        Production[] productions = GrammarScanner.scan(Test.demoGrammar);
        System.out.println("Come On!!");
    }
}
