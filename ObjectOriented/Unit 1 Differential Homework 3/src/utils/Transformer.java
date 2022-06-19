package utils;

import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Transformer {

    private static FormatAdmin formatAdmin = new FormatAdmin();

    public ArrayList<String> splitExtra(String string) {
        ArrayList<String> strings = new ArrayList<>();
        String searcher = string;
        while (extract(searcher).indexOf('+') != -1) {
            int index = extract(searcher).indexOf('+');
            strings.add(searcher.substring(0, index));
            searcher = searcher.substring(index + 1);
        }
        strings.add(searcher);
        return strings;
    }

    public String extract(String string) {
        StringBuilder sb = new StringBuilder();
        int depth = 0;
        boolean outOfBracketsFlag = true;
        for (int i = 0; i < string.length(); i++) {
            char ch = string.charAt(i);
            if (ch == '(') {
                if (depth == 0) {
                    outOfBracketsFlag = false;
                }
                depth++;
            } else if (ch == ')') {
                depth--;
                if (depth == 0) {
                    outOfBracketsFlag = true;
                    sb.append("@");
                    continue;
                }
            }
            if (!outOfBracketsFlag) {
                sb.append("@");
            } else {
                sb.append(ch);
            }
        }
        String str = sb.toString();
        sb = new StringBuilder();
        for (int i = 0; i < str.length(); i++) {
            char ch = str.charAt(i);
            if (ch == '<') {
                if (depth == 0) {
                    outOfBracketsFlag = false;
                }
                depth++;
            } else if (ch == '>') {
                depth--;
                if (depth == 0) {
                    outOfBracketsFlag = true;
                    sb.append("@");
                    continue;
                }
            }
            if (!outOfBracketsFlag) {
                sb.append("@");
            } else {
                sb.append(ch);
            }
        }
        return sb.toString();
    }

    public String concat(String part, ArrayList<String> split,
                         int left, int right) {
        int depth = 0;
        boolean limitedFlag = false;
        int leftLimit = -1;
        int rightLimit = part.length();
        for (int i = left; i > 0; i--) {
            char ch = part.charAt(i);
            if (ch == '>') {
                depth++;
            } else if (ch == '<') {
                depth--;
                if (depth < 0) {
                    limitedFlag = true;
                    leftLimit = i;
                    rightLimit = formatAdmin.findPairBracketSharp(part, leftLimit);
                    break;
                }
            }
        }
        if (!limitedFlag) {
            StringBuilder res = new StringBuilder(
                    part.substring(0, left) + split.get(0) + part.substring(right + 1));
            for (int i = 1; i < split.size(); i++) {
                res.append("+").append(part.substring(0, left))
                        .append(split.get(i)).append(part.substring(right + 1));
            }
            return res.toString();
        } else {
            StringBuilder res = new StringBuilder(part.substring(0, left) + split.get(0) +
                    part.substring(right + 1, rightLimit));
            for (int i = 1; i < split.size(); i++) {
                res.append("+").append(part.substring(leftLimit + 1, left))
                        .append(split.get(i)).append(part.substring(right + 1, rightLimit));
            }
            res.append(part.substring(rightLimit));
            return res.toString();
        }
    }

    public String unfoldSharp(String string) {
        String target = string;
        StringBuilder sb = new StringBuilder(target);
        Pattern pattern = Pattern.compile("(sin\\()|(cos\\()");
        Matcher matcher = pattern.matcher(target);
        //System.out.println(target);
        while (matcher.find()) {
            int start = matcher.start();
            int left = start + 3;
            int right = formatAdmin.findPairBracket(target, left);
            sb.setCharAt(left, '<');
            sb.setCharAt(right, '>');
        }
        target = sb.toString();
        for (int i = 0; i < target.length(); i++) {
            char ch = target.charAt(i);
            if (ch == '<') {
                int left = i;
                int right = formatAdmin.findPairBracketSharp(target, left);
                String substring = unfoldSharp(target.substring(left + 1, right));
                target = target.substring(0, left + 1) + substring
                        + target.substring(right);
            }
        }
        try {
            return unfold(target).replaceAll("<", "(").replaceAll(">", ")");
        } catch (Exception e) {
            return target.replaceAll("<", "(").replaceAll(">", ")");
        }
    }

    public String unfold(String string) {
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
        String formed = sb.toString();
        while (formed.indexOf('(') != -1) {
            ArrayList<String> parts = splitExtra(formed);
            ArrayList<String> results = new ArrayList<>();
            for (String part : parts) {
                if (part.isEmpty()) {
                    continue;
                }
                int left = part.indexOf('(');
                if (left == -1) {
                    results.add(part);
                    continue;
                }
                int right = formatAdmin.findPairBracket(part, left);
                String substring = part.substring(left + 1, right);
                ArrayList<String> split = splitExtra(substring);
                String res = concat(part, split, left, right);
                results.add(res);
            }
            sb = new StringBuilder();
            sb.append(results.get(0));
            for (int i = 1; i < results.size(); i++) {
                sb.append("+").append(results.get(i));
            }
            if (countTri(sb.toString()) > 25) {
                break;
            }
            formed = sb.toString();
        }
        return formed.replaceAll("<", "(").replaceAll(">", ")");
    }

    public String pickNeg(String string) {
        String target = string;
        Pattern pattern = Pattern.compile("(sin\\(\\|)|(cos\\(\\|)");
        Matcher matcher = pattern.matcher(target);
        while (matcher.find()) {
            int left = matcher.start() + 3;
            int right = formatAdmin.findPairBracket(target, left);
            String substring = target.substring(left + 1, right);
            StringBuilder sb = new StringBuilder();
            sb.append(target);
            if (extract(substring).indexOf('+') == -1) {
                sb.setCharAt(left + 1, target.charAt(left));
                sb.setCharAt(left, target.charAt(left - 1));
                sb.setCharAt(left - 1, target.charAt(left - 2));
                sb.setCharAt(left - 2, target.charAt(left - 3));
                if (target.charAt(matcher.start()) == 's') {
                    sb.setCharAt(left - 3, '|');
                } else {
                    sb.setCharAt(left - 3, '@');
                }
                if (isIndexEven(target, right)) {
                    sb.setCharAt(left - 3, '@');
                }
            } else {
                sb.setCharAt(left, '<');
                sb.setCharAt(right, '>');
            }
            target = sb.toString().replaceAll("@", "");
            matcher = pattern.matcher(target);
        }
        return target.replaceAll("<", "(").replaceAll(">", ")")
                .replaceAll("@", "");
    }

    private int countTri(String string) {
        Pattern pattern = Pattern.compile("(sin)|(cos)");
        Matcher matcher = pattern.matcher(string);
        int i = 0;
        while (matcher.find()) {
            i++;
        }
        return i;
    }

    private boolean isIndexEven(String target, int right) {
        try {
            if (right + 2 >= target.length()) {
                return false;
            } else if (target.charAt(right + 1) == '^') {
                boolean evenFlag = true;
                if (target.charAt(right + 2) == '|') {
                    int start = right + 3;
                    while (start < target.length() && target.charAt(start) >= '0'
                            && target.charAt(start) <= '9') {
                        evenFlag = (target.charAt(start) - '0') % 2 == 0;
                        start = start + 1;
                    }
                } else {
                    int start = right + 2;
                    while (start < target.length() && target.charAt(start) >= '0'
                            && target.charAt(start) <= '9') {
                        evenFlag = (target.charAt(start) - '0') % 2 == 0;
                        start = start + 1;
                    }
                }
                return evenFlag;
            } else {
                return false;
            }
        } catch (Exception e) {
            return false;
        }
    }
}
