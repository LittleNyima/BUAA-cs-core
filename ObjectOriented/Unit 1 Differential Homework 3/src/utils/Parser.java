package utils;

import interfaces.FactorType;
import structures.Expression;
import structures.Term;
import structures.factors.ConstFactor;
import structures.factors.CosFactor;
import structures.factors.ExFactor;
import structures.factors.PowerFactor;
import structures.factors.SinFactor;

import java.math.BigInteger;
import java.util.ArrayList;

public class Parser {

    private static FormatAdmin formatAdmin = new FormatAdmin();

    private String raw;
    private String done;
    private ArrayList<Parser> subs;

    public Parser(String raw) {
        this.raw = raw;
        this.subs = new ArrayList<>();
        this.done = this.extract();
    }

    public String extract() {
        StringBuilder sb = new StringBuilder();
        int head = 0;
        int tail;
        int depth = 0;
        boolean outOfBracketsFlag = true;
        for (int i = 0; i < raw.length(); i++) {
            char ch = raw.charAt(i);
            if (ch == '(') {
                depth++;
                if (depth == 1) {
                    head = i;
                    outOfBracketsFlag = false;
                    sb.append((char) ('A' + subs.size()));
                }
            } else if (ch == ')') {
                depth--;
                if (depth == 0) {
                    tail = i;
                    outOfBracketsFlag = true;
                    String substring = raw.substring(head + 1, tail);
                    subs.add(new Parser(substring));
                }
            } else if (outOfBracketsFlag) {
                sb.append(ch);
            }
        }
        return sb.toString();
    }

    public SinFactor parseSin(String factor) {
        String[] split = factor.replaceAll("\\|", "-").split("\\^");
        for (int i = 0; i < factor.length(); i++) {
            char ch = factor.charAt(i);
            if (ch >= 'A' && ch <= 'Z') {
                int ord = ch - 'A';
                try {
                    String innerStr = subs.get(ord).done;
                    if (split.length == 2) {
                        return new SinFactor(subs.get(ord).parseFactor(innerStr),
                                new BigInteger(split[1]));
                    } else {
                        return new SinFactor(subs.get(ord).parseFactor(innerStr), BigInteger.ONE);
                    }
                } catch (Exception e) {
                    String innerStr = subs.get(ord).raw;
                    if (split.length == 2) {
                        return new SinFactor(subs.get(ord).parseFactor(innerStr),
                                new BigInteger(split[1]));
                    } else {
                        return new SinFactor(subs.get(ord).parseFactor(innerStr), BigInteger.ONE);
                    }
                }
            }
        }
        return null;
    }

    public CosFactor parseCos(String factor) {
        String[] split = factor.replaceAll("\\|", "-").split("\\^");
        for (int i = 0; i < factor.length(); i++) {
            char ch = factor.charAt(i);
            if (ch >= 'A' && ch <= 'Z') {
                int ord = ch - 'A';
                try {
                    String innerStr = subs.get(ord).raw;
                    if (split.length == 2) {
                        return new CosFactor(subs.get(ord).parseFactor(innerStr),
                                new BigInteger(split[1]));
                    } else {
                        return new CosFactor(subs.get(ord).parseFactor(innerStr), BigInteger.ONE);
                    }
                } catch (Exception e) {
                    String innerStr = subs.get(ord).done;
                    if (split.length == 2) {
                        return new CosFactor(subs.get(ord).parseFactor(innerStr),
                                new BigInteger(split[1]));
                    } else {
                        return new CosFactor(subs.get(ord).parseFactor(innerStr), BigInteger.ONE);
                    }
                }
            }
        }
        return null;
    }

    public FactorType parseFactor(String factor) {
        int type = formatAdmin.factorType(factor);
        String[] split = factor.replaceAll("\\|", "-").split("\\^");
        switch (type) {
            case 1:
                return new ConstFactor(factor.replaceAll("\\|", "-"));
            case 2:
                if (split.length == 2) {
                    return new PowerFactor(split[1]);
                } else {
                    return new PowerFactor(BigInteger.ONE);
                }
            case 3:
                return parseSin(factor);
            case 4:
                return parseCos(factor);
            case 5:
                return new ExFactor(subs.get(factor.charAt(0) - 'A').generate());
            default:
                return new ExFactor(new Parser(factor).generate());
        }
    }

    public Expression generate() {
        Expression ret = new Expression();
        String[] terms = done.split("\\+");
        for (String term : terms) {
            if (term.isEmpty()) {
                continue;
            }
            Term termType = new Term();
            String[] factors = term.split("\\*");
            for (String factor : factors) {
                if (factor.isEmpty()) {
                    continue;
                }
                if (factor.charAt(0) == '|') {
                    termType.addFactor(new ConstFactor("-1"));
                    factor = factor.substring(1);
                }
                FactorType factorType = parseFactor(factor);
                termType.addFactor(factorType);
            }
            ret.addTerm(termType);
        }
        return ret;
    }
}
