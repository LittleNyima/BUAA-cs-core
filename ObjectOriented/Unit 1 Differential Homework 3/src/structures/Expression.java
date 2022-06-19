package structures;

import java.util.ArrayList;

public class Expression {

    private ArrayList<Term> terms;

    public Expression() {
        this.terms = new ArrayList<>();
    }

    public Expression(ArrayList<Term> terms) {
        this.terms = terms;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        int skip = -1;
        for (int i = 0; i < terms.size(); i++) {
            Term term = terms.get(i);
            if (/*!term.withSign() &&*/ !term.isEmpty()) {
                sb.append(term.toString());
                skip = i;
                break;
            }
        }
        for (int i = 0; i < terms.size(); i++) {
            if (i != skip) {
                Term term = terms.get(i);
                if (term.isEmpty()) {
                    continue;
                }
                /*if (!term.withSign())*/ {
                    sb.append("+");
                }
                sb.append(term.toString());
            }
        }
        if (sb.toString().isEmpty()) {
            return "0";
        }
        return sb.toString();
    }

    public void addTerm(Term term) {
        if (term == null) {
            return;
        }
        for (Term termType : terms) {
            if (termType.canMerge(term)) {
                termType.addUpdate(term);
                return;
            }
        }
        terms.add(term);
    }

    public Expression differential() {
        ArrayList<Term> terms = new ArrayList<>();
        for (int i = 0; i < this.terms.size(); i++) {
            terms.addAll(this.terms.get(i).differential());
        }
        return new Expression(terms);
    }
}
