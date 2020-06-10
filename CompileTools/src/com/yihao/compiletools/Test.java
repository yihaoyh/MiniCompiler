package com.yihao.compiletools;

public class Test {
    public static String demoGrammar =
            "E:T E'\n" +
            "E':+ T E' | epsilon\n" +
            "T:F T'\n" +
            "T':* F T' | epsilon\n" +
            "F:( E ) | id";
}
