package com.yihao.compiletools.common;


public class Terminator extends StringElement{

    public Terminator(String str) {
        super(str);
    }

    @Override
    public int hashCode() {
        return content.hashCode();
    }

    @Override
    public boolean equals(Object obj) {
        if(obj instanceof Terminator){
            return content.equals(((Terminator) obj).content);
        }
        return false;
    }
}
