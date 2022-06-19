import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class FormatChecker {

    private static String operator = "([+-])";
    private static String blank = "([ \t]*)";
    private static String integer = "([0-9]+)";
    private static String signedInt = "(" + operator + "?" + integer + ")";
    private static String index = "(\\*\\*" + blank + signedInt + ")";
    private static String sin = "(sin" + blank + "\\(" + blank + "x" + blank + "\\)"
            + "(" + blank + index + ")?)";
    private static String cos = "(cos" + blank + "\\(" + blank + "x" + blank + "\\)"
            + "(" + blank + index + ")?)";
    private static String triangle = "(" + sin + "|" + cos + ")";
    private static String power = "(x(" + blank + index + ")?)";
    private static String variable = "(" + triangle + "|" + power + ")";
    private static String factor = "(" + variable + "|" + signedInt + ")";
    private static String basicTerm = "((" + operator + blank + ")?" + factor + ")";
    private static String term = "(" + basicTerm + "(" + blank + "\\*" + blank + factor + ")*)";
    private static String expression = blank + "(" + operator + blank + ")?" + term + blank
            + "(" + operator + blank + term + blank + ")*";
    private static Pattern pattern = Pattern.compile(expression);
    private Matcher matcher;

    FormatChecker(String string) {
        this.matcher = pattern.matcher(string);
    }

    public boolean matches() {
        return this.matcher.matches();
    }
}
