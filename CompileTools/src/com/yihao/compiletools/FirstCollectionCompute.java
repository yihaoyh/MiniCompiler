package com.yihao.compiletools;

import com.yihao.compiletools.common.*;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;

public class FirstCollectionCompute {
    public static FirstCollection[] calculateFirst(Production[] productions){
        // 对所有产生式
        // 对产生式的所有可能结果
        // 如果第一个为终结符，则添加到First集中
        // 如果第一个为非终结符B，则将First(B)添加到集合中，如果B可以推导出epsilon，则对下一个非终结符处理
        // 如果没有新的终结符增加，则退出循环
        HashMap<Nonterminal, HashSet<Terminator>> collections = new HashMap<>();
        for (int i = 0; i < productions.length; i++) {
            collections.put(productions[i].left, new HashSet<>());
        }
        boolean isChanged = false;
        do {
            isChanged = false;
            for (int i = 0; i < productions.length; i++) {
                Production production = productions[i];
                HashSet<Terminator> target = collections.get(production.left);
                int lastSize = target.size();
                for (ArrayList<StringElement> stringElements : production.right) {
                    for (StringElement element : stringElements) {
                        if (element instanceof Terminator) {
                            target.add((Terminator) element);
                            break;
                        } else {
                            Nonterminal nonterminal = (Nonterminal) element;
                            target.addAll(collections.get(nonterminal));
                            if (!collections.get(nonterminal).contains(new Terminator("epsilon"))) {
                                break;
                            }
                        }
                    }
                }
                if(target.size() != lastSize){
                    isChanged = true;
                }
            }
        }while (isChanged);
        ArrayList<FirstCollection> result = new ArrayList<>();
        for (Nonterminal nonterminal : collections.keySet()) {
            FirstCollection first = new FirstCollection();
            first.nonterminal = nonterminal;
            first.terminators = collections.get(nonterminal).toArray(new Terminator[0]);
            result.add(first);
        }
        return result.toArray(new FirstCollection[0]);
    }
}
