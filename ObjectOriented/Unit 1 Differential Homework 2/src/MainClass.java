import java.util.Scanner;

public class MainClass {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        String line = scanner.nextLine();
        FormatChecker formatChecker = new FormatChecker(line);
        if (!formatChecker.matches()) {
            System.out.println("WRONG FORMAT!");
            return;
        }
        Parser parser = new Parser(line);
        ExpressionType expression = parser.generate();
        if (expression == null) {
            System.out.println("WRONG FORMAT!");
            return;
        }
        Optimizer optimizer = new Optimizer();
        ExpressionType diffEx = expression.differential();
        System.out.println(optimizer.complexOptimize(diffEx).toString());
    }
}
