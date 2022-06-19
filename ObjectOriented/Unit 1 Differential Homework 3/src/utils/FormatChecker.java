package utils;

import exceptions.WrongFormatException;

import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class FormatChecker {

    private String raw;
    private String done;
    private ArrayList<FormatChecker> subs;

    public FormatChecker(String raw) throws WrongFormatException {
        this.raw = raw;
        this.subs = new ArrayList<>();
        this.done = extract();
    }

    public String extract() throws WrongFormatException {
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
                    subs.add(new FormatChecker(substring));
                } else if (depth < 0) {
                    throw new WrongFormatException("BracketsNotPair");
                }
            } else if (outOfBracketsFlag) {
                sb.append(ch);
            }
        }
        if (depth > 0) {
            throw new WrongFormatException("BracketsNotPair");
        }
        return sb.toString();
    }

    public boolean isLegal() throws WrongFormatException {
        if (includeIllegalChar()) {
            throw new WrongFormatException("IllegalChar");
        }
        FormatAdmin formatAdmin = new FormatAdmin();
        boolean isLegal = formatAdmin.matches(this.done);
        for (FormatChecker sub : subs) {
            isLegal = isLegal && sub.isLegal();
        }
        Pattern pattern = Pattern.compile("(sin[ \t]*[A-Z])|(cos[ \t]*[A-Z])");
        Matcher matcher = pattern.matcher(this.done);
        while (matcher.find()) {
            String found = matcher.group();
            char ord = found.charAt(found.length() - 1);
            String innerRaw = this.subs.get(ord - 'A').raw;
            String innerDone = this.subs.get(ord - 'A').done;
            isLegal = isLegal &&
                    (formatAdmin.isFactor(innerRaw) || formatAdmin.isFactor(innerDone));
        }
        String illegalBlank = "((sin)|(cos))[ \t]*\\([ \t]*[+\\-][ \t]";
        if (!isLegal || raw.replaceAll(illegalBlank, "@").indexOf('@') != -1) {
            throw new WrongFormatException();
        }
        return isLegal;
    }

    public boolean includeIllegalChar() throws WrongFormatException {
        String legalChars = "[cosinx0-9()*+\\- \t]";
        if (!raw.replaceAll(legalChars, "").isEmpty()) {
            throw new WrongFormatException("IllegalChar");
        }
        return !raw.replaceAll(legalChars, "").isEmpty();
    }
}
