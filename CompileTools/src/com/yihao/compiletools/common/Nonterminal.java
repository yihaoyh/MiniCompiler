package com.yihao.compiletools.common;
/*
 * created by hp at 2020/6/10
 * 非终结符
 */
public class Nonterminal extends StringElement{
    public Nonterminal(String str) {
        super(str);
    }

    @Override
    public int hashCode() {
        return content.hashCode();
    }

    @Override
    public boolean equals(Object obj) {
        if(obj instanceof Nonterminal){
            return content.equals(((Nonterminal) obj).content);
        }
        return false;
    }
}
