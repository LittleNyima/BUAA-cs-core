import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class MainClass {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        String expression = scanner.nextLine().replaceAll("[ \t]", "");
        String regex = "[+-]{0,2}[0-9]*\\*?x?(\\*\\*[+-]?[0-9]*)?";
        Pattern pattern = Pattern.compile(regex);
        Matcher matcher = pattern.matcher(expression);
        Polynomial polynomial = new Polynomial();
        while (matcher.find()) {
            polynomial.parseAddTerm(matcher.group());
        }
        polynomial.differential().print();
    }
}
