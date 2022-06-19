import structures.Expression;
import utils.FormatChecker;
import utils.Parser;
import utils.Simplifier;
import utils.Transformer;

import java.util.Scanner;

public class MainClass {

    private static final String wfInfo = "WRONG FORMAT!";

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        String line = scanner.nextLine();
        try {
            FormatChecker formatChecker = new FormatChecker(line);
            formatChecker.isLegal();
        } catch (Exception e) {
            System.out.println(wfInfo);
            return;
        }
        Simplifier simplifier = new Simplifier();
        Transformer transformer = new Transformer();
        String done = transformer.unfoldSharp(simplifier.execute(line));
        done = transformer.pickNeg(done);
        for (int i = 0; i < 10 && done.compareTo(simplifier.normalizeExtra(done)) != 0; i++) {
            done = simplifier.normalizeExtra(done);
        }
        Parser parser = new Parser(done);
        Expression expression = parser.generate();
        System.out.println(expression.differential());
    }
}
