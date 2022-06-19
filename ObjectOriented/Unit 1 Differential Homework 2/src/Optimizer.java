import java.util.ArrayList;

public class Optimizer {

    private ArrayList<TermType> mergeTerm(TermType term, TermType anotherTerm) {
        return term.merge(anotherTerm);
    }

    private ArrayList<TermType> convertTerm(TermType term, TermType anotherTerm) {
        return term.convert(anotherTerm);
    }

    private void mergeSiblings(ArrayList<TermType> terms) {
        for (int i = 0; i < terms.size(); i++) {
            if (terms.get(i).isEmpty()) {
                continue;
            }
            for (int j = i + 1; j < terms.size(); j++) {
                if (terms.get(j).isEmpty()) {
                    continue;
                }
                if (terms.get(i).compareTo(terms.get(j)) == 0) {
                    terms.set(i, terms.get(i).add(terms.get(j)));
                    terms.get(j).setEmpty();
                }
            }
        }
    }

    public ExpressionType simpleOptimize(ExpressionType expression) {
        ExpressionType expressionClone = (ExpressionType) expression.clone();
        ArrayList<TermType> terms = expressionClone.getTerms();
        for (int i = 0; i < expressionClone.getLength(); i++) {
            if (terms.get(i).isEmpty()) {
                continue;
            }
            for (int j = i + 1; j < expressionClone.getLength(); j++) {
                if (terms.get(j).isEmpty()) {
                    continue;
                }
                ArrayList<TermType> merged = mergeTerm(terms.get(i), terms.get(j));
                if (merged != null) {
                    if (merged.size() == 1 && merged.get(0).toString().length() <=
                            terms.get(i).toString().length() + terms.get(j).toString().length()) {
                        terms.set(i, merged.get(0));
                        terms.get(j).setEmpty();
                    } else if (merged.size() == 2 &&
                            merged.get(0).toString().length() + merged.get(1).toString().length() <=
                                    terms.get(i).toString().length()
                                            + terms.get(j).toString().length()) {
                        terms.set(i, merged.get(0));
                        terms.set(j, merged.get(1));
                    }
                }
            }
        }
        mergeSiblings(terms);
        return new ExpressionType(terms);
    }

    public ExpressionType furtherOptimize(ExpressionType expression) {
        ExpressionType expressionClone = (ExpressionType) expression.clone();
        ArrayList<TermType> terms = expressionClone.getTerms();
        for (int i = 0; i < expressionClone.getLength(); i++) {
            if (terms.get(i).isEmpty()) {
                continue;
            }
            for (int j = i + 1; j < expressionClone.getLength(); j++) {
                if (terms.get(j).isEmpty()) {
                    continue;
                }
                ArrayList<TermType> converted = convertTerm(terms.get(i), terms.get(j));
                if (converted != null) {
                    if (converted.size() == 1 && converted.get(0).toString().length() <=
                            terms.get(i).toString().length() + terms.get(j).toString().length()) {
                        terms.set(i, converted.get(0));
                        terms.get(j).setEmpty();
                    } else if (converted.size() == 2 &&
                            converted.get(0).toString().length()
                                    + converted.get(1).toString().length() <=
                                    terms.get(i).toString().length()
                                            + terms.get(j).toString().length()) {
                        terms.set(i, converted.get(0));
                        terms.set(j, converted.get(1));
                    }
                }
            }
        }
        mergeSiblings(terms);
        return new ExpressionType(terms);
    }

    public ExpressionType complexOptimize(ExpressionType expression) {
        ExpressionType simplified = this.simpleOptimize((ExpressionType) expression.clone());
        ExpressionType further = this.furtherOptimize((ExpressionType) expression.clone());
        ExpressionType sf = this.furtherOptimize((ExpressionType) simplified.clone());
        ExpressionType fs = this.simpleOptimize((ExpressionType) further.clone());
        ExpressionType returnEx = simplified;
        if (returnEx.toString().length() > further.toString().length()) {
            returnEx = further;
        }
        if (returnEx.toString().length() > sf.toString().length()) {
            returnEx = sf;
        }
        if (returnEx.toString().length() > fs.toString().length()) {
            returnEx = fs;
        }
        return returnEx;
    }
}
