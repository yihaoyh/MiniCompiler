package com.yihao.compiletools;

import com.yihao.compiletools.common.FirstCollection;
import com.yihao.compiletools.common.Production;

public class Main {

    public static void main(String[] args) {
        Production[] productions = GrammarScanner.scan(Test.demoGrammar);
        FirstCollection[] firsts = FirstCollectionCompute.calculateFirst(productions);
        System.out.println("Come On!! Keep Going!!");
    }
}
