package utils;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Simplifier {

    private FormatAdmin formatAdmin = new FormatAdmin();

    public String execute(String string) {
        return stripSingleFactor(stripBracket(normalize(string)));
    }

    public String normalize(String string) {
        return string.replaceAll("[ \t]", "")
                .replaceAll("\\+\\+\\+", "+")
                .replaceAll("\\+\\+-", "-")
                .replaceAll("\\+-\\+", "-")
                .replaceAll("\\+--", "+")
                .replaceAll("-\\+\\+", "-")
                .replaceAll("-\\+-", "+")
                .replaceAll("--\\+", "+")
                .replaceAll("---", "-")
                .replaceAll("\\+\\+", "+")
                .replaceAll("\\+-", "-")
                .replaceAll("-\\+", "-")
                .replaceAll("--", "+")
                .replaceAll("\\*\\*\\+", "^")
                .replaceAll("\\*\\*-", "^|")
                .replaceAll("\\*\\*", "^")
                .replaceAll("\\*\\+", "*")
                .replaceAll("\\*-", "*|")
                .replaceAll("-", "+|")
                .replaceAll("^\\+", "")
                .replaceAll("\\(\\+", "(");
    }

    public String normalizeExtra(String string) {
        String ret = string.replaceAll("\\+\\+\\+", "+")
                .replaceAll("\\+\\+\\|", "-")
                .replaceAll("\\+\\|\\+", "-")
                .replaceAll("\\+\\|\\|", "+")
                .replaceAll("\\|\\+\\+", "-")
                .replaceAll("\\|\\+\\|", "+")
                .replaceAll("\\|\\|\\+", "+")
                .replaceAll("\\|\\|\\|", "-")
                .replaceAll("\\+\\+", "+")
                .replaceAll("\\+\\|", "-")
                .replaceAll("\\|\\+", "-")
                .replaceAll("\\|\\|", "+")
                .replaceAll("\\*\\+", "*")
                .replaceAll("-", "|")
                .replaceAll("\\|", "+|")
                .replaceAll("\\*\\+", "*")
                .replaceAll("\\^\\+", "^")
                .replaceAll("^\\+", "")
                .replaceAll("\\(\\+", "(");
        Pattern neg = Pattern.compile("\\|[^0-9]");
        Matcher matcher = neg.matcher(ret);
        while (matcher.find()) {
            if (matcher.start() > 0) {
                ret = ret.substring(0, matcher.start()) + "|1*"
                        + ret.substring(matcher.start() + 1);
                matcher = neg.matcher(ret);
            }
        }
        return ret;
    }

    public String stripBracket(String string) {
        String ret = string;
        while (formatAdmin.findPairBracket(ret, 0) == ret.length() - 1) {
            ret = ret.substring(1, ret.length() - 1);
        }
        StringBuilder sb = new StringBuilder();
        sb.append(ret);
        for (int i = 0; i < ret.length() - 1; i++) {
            if (ret.charAt(i) == '(' && ret.charAt(i + 1) == '(') {
                int outer = formatAdmin.findPairBracket(ret, i);
                int inner = formatAdmin.findPairBracket(ret, i + 1);
                if (inner + 1 == outer) {
                    sb.setCharAt(i + 1, '@');
                    sb.setCharAt(inner, '@');
                }
            }
        }
        /*Pattern pattern = Pattern.compile("\\(\\(([^()]+)\\)\\)");
        Matcher matcher = pattern.matcher(ret);
        while (matcher.find()) {
            ret = ret.replace(matcher.group(0), "(" + matcher.group(1) + ")");
            matcher = pattern.matcher(ret);
        }*/
        return sb.toString().replaceAll("@", "");
    }

    public String stripSingleFactor(String string) {
        StringBuilder sb = new StringBuilder(string);
        Pattern pattern = Pattern.compile("(sin)|(cos)");
        Matcher matcher = pattern.matcher(string);
        while (matcher.find()) {
            int start = matcher.start();
            int left = start + 3;
            int right = formatAdmin.findPairBracket(string, left);
            sb.setCharAt(left, '<');
            sb.setCharAt(right, '>');
        }
        for (int i = 0; i < string.length(); i++) {
            char ch = sb.toString().charAt(i);
            if (ch == '(') {
                int right = formatAdmin.findPairBracket(string, i);
                if (formatAdmin.isSingleTerm(string.substring(i + 1, right))) {
                    sb.setCharAt(i, '@');
                    sb.setCharAt(right, '@');
                }
            }
        }
        return sb.toString().replaceAll("@", "")
                .replaceAll("<", "(")
                .replaceAll(">", ")");
    }
}
