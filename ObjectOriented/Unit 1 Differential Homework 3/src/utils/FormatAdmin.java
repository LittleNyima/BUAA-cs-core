package utils;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class FormatAdmin {

    private static final String operator = "([+-])";
    private static final String blank = "([ \t]*)";
    private static final String number = "([0-9]+)";
    private static final String limitedNum = "((0*[1-4]?[0-9])|(0*50))";
    private static final String signed = "(" + operator + "?" + number + ")";
    private static final String limitedSigned = "(" + operator + "?" + limitedNum + ")";
    private static final String index = "(\\*\\*" + blank + limitedSigned + ")";
    private static final String power = "(x(" + blank + index + ")?)";
    private static final String exFactor = "([A-Z])";
    private static final String sin = "(sin" + blank + exFactor + "(" + blank + index + ")?)";
    private static final String cos = "(cos" + blank + exFactor + "(" + blank + index + ")?)";
    private static final String triangle = "(" + sin + "|" + cos + ")";
    private static final String variable = "(" + power + "|" + triangle + ")";
    private static final String factor = "(" + variable + "|" + signed + "|" + exFactor + ")";
    private static final String base = "((" + operator + blank + ")?" + factor + ")";
    private static final String term = "(" + base + "(" + blank + "\\*" + blank + factor + ")*)";
    private static final String expression = blank + "(" + operator + blank + ")?" + term + blank
            + "(" + operator + blank + term + blank + ")*";
    private static final Pattern pattern = Pattern.compile(expression);
    private static final Pattern factorPattern = Pattern.compile(factor);

    private static final Pattern constFactor = Pattern.compile("[+|]?[0-9]+");
    private static final Pattern powerFactor = Pattern.compile("[+|]?x(\\^[+|]?[0-9]+)?");
    private static final Pattern sinFactor = Pattern.compile("[+|]?sin[A-Z](\\^[+|]?[0-9]+)?");
    private static final Pattern cosFactor = Pattern.compile("[+|]?cos[A-Z](\\^[+|]?[0-9]+)?");
    private static final Pattern expFactor = Pattern.compile("[A-Z]");

    private Matcher matcher;

    public boolean matches(String string) {
        this.matcher = pattern.matcher(string);
        return this.matcher.matches();
    }

    public boolean isFactor(String string) {
        matcher = factorPattern.matcher(string.replaceAll("[ \t]", ""));
        return matcher.matches();
    }

    public int findPairBracket(String string, int index) {
        if (string.charAt(index) != '(') {
            return -1;
        }
        int depth = 0;
        for (int i = index; i < string.length(); i++) {
            char ch = string.charAt(i);
            if (ch == '(') {
                depth++;
            } else if (ch == ')') {
                depth--;
                if (depth == 0) {
                    return i;
                }
            }
        }
        return -1;
    }

    public int findPairBracketSharp(String string, int index) {
        if (string.charAt(index) != '<') {
            return -1;
        }
        int depth = 0;
        for (int i = index; i < string.length(); i++) {
            char ch = string.charAt(i);
            if (ch == '<') {
                depth++;
            } else if (ch == '>') {
                depth--;
                if (depth == 0) {
                    return i;
                }
            }
        }
        return -1;
    }

    public int factorType(String factor) {
        Matcher matcher = constFactor.matcher(factor);
        if (matcher.matches()) {
            return 1;
        }
        matcher = powerFactor.matcher(factor);
        if (matcher.matches()) {
            return 2;
        }
        matcher = sinFactor.matcher(factor);
        if (matcher.matches()) {
            return 3;
        }
        matcher = cosFactor.matcher(factor);
        if (matcher.matches()) {
            return 4;
        }
        matcher = expFactor.matcher(factor);
        if (matcher.matches()) {
            return 5;
        }
        matcher = pattern.matcher(factor);
        if (matcher.matches()) {
            return 6;
        }
        return -1;
    }

    public boolean isSingleTerm(String string) {
        return string.replaceAll("\\(*?\\)", "").indexOf('+') == -1;
    }
}
