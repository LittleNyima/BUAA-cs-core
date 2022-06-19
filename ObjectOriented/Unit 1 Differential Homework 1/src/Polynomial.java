import java.math.BigInteger;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Polynomial {

    private ArrayList<BigInteger> coefficients;
    private ArrayList<BigInteger> powers;
    private int length;

    Polynomial() {
        this.coefficients = new ArrayList<>();
        this.powers = new ArrayList<>();
        this.length = 0;
    }

    public void addTerm(BigInteger coefficient, BigInteger power) {
        if (length == 0) {
            coefficients.add(coefficient);
            powers.add(power);
            length = length + 1;
            return;
        }
        for (int i = 0; i < length; i++) {
            BigInteger oldCoefficient = coefficients.get(i);
            BigInteger oldPower = powers.get(i);
            if (power.compareTo(oldPower) == 0) {
                BigInteger newCoefficient = oldCoefficient.add(coefficient);
                coefficients.set(i, newCoefficient);
                return;
            }
        }
        coefficients.add(coefficient);
        powers.add(power);
        length = length + 1;
    }

    public void parseAddTerm(String term) {
        if (term.isEmpty()) {
            return;
        }
        String[] split = term.split("x");
        if (split.length == 1) { /* the case where the term is a constance or
            the term ends or starts with "x"
            */
            if (term.endsWith("x")) {
                this.addTerm(parseCoefficient(split[0]), new BigInteger("1"));
            } else if (term.startsWith("x")) {
                this.addTerm(new BigInteger("1"), parsePower(split[0]));
            } else {
                this.addTerm(parseCoefficient(split[0]), new BigInteger("0"));
            }
        } else if (split.length == 2) { // the other case
            this.addTerm(parseCoefficient(split[0]), parsePower(split[1]));
        } else if (split.length == 0) { // the term is "x"
            this.addTerm(new BigInteger("1"), new BigInteger("1"));
        } else {
            System.out.println("Error: ParseError");
        }
    }

    private BigInteger parseCoefficient(String term) {
        String string = "";
        Pattern pattern = Pattern.compile("[+-]{1,2}");
        Matcher matcher = pattern.matcher(term);
        if (matcher.find()) {
            String found = matcher.group();
            if (found.compareTo("-") == 0 || found.compareTo("+-") == 0 ||
                    found.compareTo("-+") == 0) {
                string = string + "-";
            }
        }
        pattern = Pattern.compile("[0-9]+");
        matcher = pattern.matcher(term);
        if (matcher.find() && !matcher.group().isEmpty()) {
            string = string + matcher.group();
        } else {
            string = string + "1";
        }
        return new BigInteger(string);
    }

    private BigInteger parsePower(String term) {
        String string;
        Pattern pattern = Pattern.compile("[+-]?[0-9]+");
        Matcher matcher = pattern.matcher(term);
        if (matcher.find()) {
            string = matcher.group();
        } else {
            string = "1";
        }
        return new BigInteger(string);
    }

    public Polynomial differential() {
        Polynomial polynomial = new Polynomial();
        for (int i = 0; i < length; i++) {
            BigInteger coefficient = coefficients.get(i);
            BigInteger power = powers.get(i);
            if (coefficient.compareTo(new BigInteger("0")) != 0 &&
                    power.compareTo(new BigInteger("0")) != 0) {
                BigInteger newCoefficient = coefficient.multiply(power);
                BigInteger newPower = power.subtract(new BigInteger("1"));
                polynomial.addTerm(newCoefficient, newPower);
            }
        }
        return polynomial;
    }

    /* print polynomial
     * the print process follows principles as follow:
     * 1. ignore space, \t and ``1``s
     * 2. merge as much as possible
     * 3. the first term should be positive if possible
     */
    public void print() {
        ArrayList<BigInteger> coefficientsClone = new ArrayList<>();
        for (int i = 0; i < length; i++) {
            coefficientsClone.add(coefficients.get(i));
        }
        String output = "";
        for (int i = 0; i < length; i++) { // find a positive term and output
            BigInteger coefficient = coefficientsClone.get(i);
            BigInteger power = powers.get(i);
            if (coefficient.compareTo(new BigInteger("0")) > 0) {
                if (power.compareTo(new BigInteger("0")) == 0) {
                    output = output + coefficient.toString();
                } else {
                    if (coefficient.compareTo(new BigInteger("1")) != 0) {
                        output = output + coefficient.toString() + "*";
                    }
                    output = output + "x";
                    if (power.compareTo(new BigInteger("1")) != 0) {
                        output = output + "**" + power.toString();
                    }
                }
                coefficientsClone.set(i, new BigInteger("0"));
                break;
            }
        }
        for (int i = 0; i < length; i++) {
            BigInteger coefficient = coefficientsClone.get(i);
            BigInteger power = powers.get(i);
            if (coefficient.compareTo(new BigInteger("0")) != 0) {
                if (power.compareTo(new BigInteger("0")) == 0) { // constance
                    if (!output.isEmpty() &&
                            coefficient.compareTo(new BigInteger("0")) > 0) {
                        output = output + "+";
                    }
                    output = output + coefficient.toString();
                } else { // variable term
                    if (coefficient.compareTo(new BigInteger("0")) > 0) {
                        output = output + "+";
                    } else {
                        output = output + "-";
                    }
                    if (coefficient.abs().compareTo(new BigInteger("1")) != 0) {
                        output = output + coefficient.abs().toString() + "*";
                    }
                    output = output + "x";
                    if (power.compareTo(new BigInteger("1")) != 0) {
                        output = output + "**" + power.toString();
                    }
                }
            }
        }
        if (output.length() == 0) {
            output = "0";
        }
        System.out.println(output);
    }
}
