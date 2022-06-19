import java.math.BigInteger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Parser {

    private String string;
    private String standard;

    Parser(String string) {
        this.string = string;
        this.standard = this.normalize();
    }

    public String normalize() {
        return this.string.replaceAll("[ \t]", "")
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
                .replaceAll("-", "+|");
    }

    // classify factors
    // 1 for numbers, 2 for powers, 3 for sin, 4 for cos, -1 otherwise
    private int factorType(String factor) {
        Pattern pattern = Pattern.compile("^[+|]?[0-9]+$");
        Matcher matcher = pattern.matcher(factor);
        if (matcher.matches()) {
            return 1;
        }
        pattern = Pattern.compile("^[+|]?x(\\^[+|]?[0-9]+)?$");
        matcher = pattern.matcher(factor);
        if (matcher.matches()) {
            return 2;
        }
        pattern = Pattern.compile("^[+|]?sin\\(x\\)(\\^[+|]?[0-9]+)?$");
        matcher = pattern.matcher(factor);
        if (matcher.matches()) {
            return 3;
        }
        pattern = Pattern.compile("^[+|]?cos\\(x\\)(\\^[+|]?[0-9]+)?$");
        matcher = pattern.matcher(factor);
        if (matcher.matches()) {
            return 4;
        }
        System.out.println("log: error occurred when parsing");
        return -1;
    }

    public ExpressionType generate() {
        ExpressionType returnEx = new ExpressionType();
        String[] terms = standard.split("\\+");
        for (String term : terms) {
            //System.out.println("** TERM : " + term);
            if (!term.isEmpty()) {
                BigInteger coefficient = BigInteger.ONE;
                BigInteger varIndex = BigInteger.ZERO;
                BigInteger sinIndex = BigInteger.ZERO;
                BigInteger cosIndex = BigInteger.ZERO;
                String[] factors = term.split("\\*");
                for (String factor : factors) {
                    if (!factor.isEmpty()) {
                        //System.out.println("* FACTOR: " + factor + ", " + factorType(factor));
                        if (factor.charAt(0) == '|') {
                            coefficient = coefficient.negate();
                        }
                        if (factorType(factor) == 1) {
                            coefficient = coefficient.multiply(new BigInteger(
                                    factor.replaceAll("\\|", "")));
                        } else if (factorType(factor) == 2) {
                            if (factor.split("\\^").length == 2) {
                                BigInteger index = new BigInteger(
                                        factor.split("\\^")[1].replaceAll("\\|", "-"));
                                if (index.abs().compareTo(new BigInteger("10000")) > 0) {
                                    return null;
                                }
                                varIndex = varIndex.add(index);
                            } else {
                                varIndex = varIndex.add(BigInteger.ONE);
                            }
                        } else if (factorType(factor) == 3) {
                            if (factor.split("\\^").length == 2) {
                                sinIndex = sinIndex.add(new BigInteger(
                                        factor.split("\\^")[1].replaceAll("\\|", "-")
                                ));
                            } else {
                                sinIndex = sinIndex.add(BigInteger.ONE);
                            }
                        } else if (factorType(factor) == 4) {
                            if (factor.split("\\^").length == 2) {
                                cosIndex = cosIndex.add(new BigInteger(
                                        factor.split("\\^")[1].replaceAll("\\|", "-")
                                ));
                            } else {
                                cosIndex = cosIndex.add(BigInteger.ONE);
                            }
                        }
                    }
                }
                returnEx.addTerm(new TermType(coefficient,
                        varIndex, sinIndex, cosIndex));
                //System.out.println("*** ADD : " + new TermType(coefficient,
                //        varIndex, sinIndex, cosIndex).toString());
            }
        }
        return returnEx;
    }
}
