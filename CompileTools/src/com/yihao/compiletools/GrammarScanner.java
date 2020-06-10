package com.yihao.compiletools;

import com.yihao.compiletools.common.Nonterminal;
import com.yihao.compiletools.common.Production;
import com.yihao.compiletools.common.StringElement;
import com.yihao.compiletools.common.Terminator;

import java.util.ArrayList;

public class GrammarScanner {
    /**
     * 扫描字符串，返回产生式数组
     * @param content
     * @return
     */
    public static Production[] scan(String content){
        // 扫描步骤
        // 1 用"\n"作为分割符，分割成多行字符串
        // 2 对每行字符串，用":"分割，左侧为非终结符，右侧为产生式
        // 3 对每个产生式，用空格分割，大写字符开头的为非终结符，其他终结符
        String[] lines = content.split("\n");
        ArrayList<Production> list = new ArrayList<>();
        for (String line : lines) {
            int splitIndex = line.indexOf(":");
            String left = line.substring(0, splitIndex);
            String right = line.substring(splitIndex + 1);
            String[] possibleElems = right.split("\\|");
            ArrayList<ArrayList<StringElement>> collections = new ArrayList<>();
            for (String possibleElem : possibleElems) {
                String[] elems = possibleElem.trim().split(" ");
                ArrayList<StringElement> elements = new ArrayList<>();
                for (String elem : elems) {
                    if(Character.isUpperCase(elem.toCharArray()[0])){
                        elements.add(new Nonterminal(elem));
                    }
                    else{
                        elements.add(new Terminator(elem));
                    }
                }
                collections.add(elements);
            }
            Production production = new Production();
            production.left = new Nonterminal(left);
            production.right = collections.toArray(new ArrayList[0]);
            list.add(production);
        }
        return list.toArray(new Production[0]);
    }
}
