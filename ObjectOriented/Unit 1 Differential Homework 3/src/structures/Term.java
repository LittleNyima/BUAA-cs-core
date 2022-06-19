package structures;

import interfaces.FactorType;
import structures.factors.ConstFactor;
import structures.factors.PowerFactor;

import java.math.BigInteger;
import java.util.ArrayList;

public class Term {

    private ArrayList<FactorType> factors;

    public Term() {
        this.factors = new ArrayList<>();
        factors.add(new ConstFactor(BigInteger.ONE));
        factors.add(new PowerFactor(BigInteger.ZERO));
    }

    @Override
    public String toString() {
        if (this.isEmpty()) {
            return "0";
        }
        StringBuilder sb = new StringBuilder();
        int skip = -1;
        for (int i = 0; i < factors.size(); i++) {
            FactorType factor = factors.get(i);
            if (!factor.isEmpty() && !factor.withSign()) {
                sb.append(factor.toString());
                sb.append("*");
                skip = i;
                break;
            }
        }
        for (int i = 0; i < factors.size(); i++) {
            FactorType factor = factors.get(i);
            if (!factor.isEmpty() && i != skip) {
                sb.append(factor.toString());
                sb.append("*");
            }
        }
        String string = sb.toString();
        if (string.isEmpty()) {
            return "1";
        }
        return string.substring(0, string.length() - 1);
    }

    public boolean isEmpty() {
        return ((ConstFactor) this.factors.get(0)).isZero();
    }

    public boolean withSign() {
        return this.toString().charAt(0) == '-' || this.toString().charAt(0) == '+';
    }

    public boolean isPower() {
        boolean isPower = true;
        if (this.factors.size() > 2) {
            for (int i = 2; i < factors.size(); i++) {
                isPower = isPower && factors.get(i).isEmpty();
            }
        }
        return isPower;
    }

    public boolean canMerge(Term another) {
        if (this.factors.size() < 2 || another.factors.size() < 2) {
            return false;
        }
        return this.isPower() && another.isPower() &&
                this.factors.get(1).equals(another.factors.get(1));
    }

    public void addFactor(FactorType factor) {
        if (factor.isEmpty()) {
            return;
        }
        for (FactorType factorType : factors) {
            if (factor.canMerge(factorType)) {
                factorType.mulUpdate(factor);
                return;
            }
        }
        factors.add(factor);
    }

    public void addUpdate(Term another) {
        ((ConstFactor) this.factors.get(0)).addUpdate((ConstFactor) another.factors.get(0));
    }

    public ArrayList<Term> differential() {
        ArrayList<Term> ret = new ArrayList<>();
        for (int i = 0; i < this.factors.size(); i++) {
            FactorType factor = factors.get(i);
            if (factor.isEmpty()) {
                continue;
            }
            ArrayList<FactorType> diff = factor.differential();
            Term term = new Term();
            for (FactorType factorType : diff) {
                term.addFactor(factorType);
            }
            for (int j = 0; j < this.factors.size(); j++) {
                if (i != j) {
                    term.addFactor(factors.get(j));
                }
            }
            ret.add(term);
        }
        return ret;
    }
}
