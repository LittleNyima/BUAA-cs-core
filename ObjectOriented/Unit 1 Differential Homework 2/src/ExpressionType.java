import java.util.ArrayList;

public class ExpressionType implements Cloneable {

    private ArrayList<TermType> terms;
    private int length;

    ExpressionType() {
        this.terms = new ArrayList<>();
        this.length = 0;
    }

    ExpressionType(ArrayList<TermType> terms) {
        this.terms = new ArrayList<>(terms);
        this.length = terms.size();
    }

    ExpressionType(ArrayList<TermType> terms, int length) {
        this.terms = new ArrayList<>(terms);
        this.length = length;
    }

    @Override
    public String toString() {
        ArrayList<TermType> termsClone = new ArrayList<>();
        for (int i = 0; i < length; i++) {
            termsClone.add((TermType) terms.get(i).clone());
        }
        StringBuffer stringBuffer = new StringBuffer();
        TermType termType;
        for (int i = 0; i < length; i++) { // find a positive term first
            termType = termsClone.get(i);
            if (termType.isPositive()) {
                stringBuffer.append(termType.toString());
                termType.setEmpty();
                break;
            }
        }
        for (int i = 0; i < length; i++) {
            termType = termsClone.get(i);
            if (!termType.isEmpty()) {
                if (termType.isPositive() && !stringBuffer.toString().isEmpty()) {
                    stringBuffer.append("+");
                }
                stringBuffer.append(termType.toString());
            }
        }
        if (stringBuffer.toString().isEmpty()) {
            stringBuffer.append("0");
        }
        return stringBuffer.toString();
    }

    @Override
    public Object clone() {
        ArrayList<TermType> termsClone = new ArrayList();
        for (int i = 0; i < terms.size(); i++) {
            termsClone.add((TermType) terms.get(i).clone());
        }
        return new ExpressionType(termsClone, length);
    }

    public ArrayList<TermType> getTerms() {
        return terms;
    }

    public int getLength() {
        return length;
    }

    public void addTerm(TermType term) {
        if (length == 0) {
            terms.add(term);
            length = length + 1;
            return;
        }
        for (int i = 0; i < length; i++) {
            TermType combination = terms.get(i).add(term);
            if (combination != null) {
                terms.set(i, combination);
                return;
            }
        }
        terms.add(term);
        length = length + 1;
        return;
    }

    public ExpressionType differential() {
        ExpressionType returnEx = new ExpressionType();
        for (int i = 0; i < length; i++) {
            ArrayList<TermType> tempArr = this.terms.get(i).differential();
            for (int j = 0; j < tempArr.size(); j++) {
                TermType termType = tempArr.get(j);
                if (!termType.isEmpty()) {
                    returnEx.addTerm(termType);
                }
            }
        }
        return returnEx;
    }
}
