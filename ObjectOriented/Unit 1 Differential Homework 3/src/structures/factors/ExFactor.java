package structures.factors;

import interfaces.FactorType;
import structures.Expression;

import java.util.ArrayList;

public class ExFactor implements FactorType {

    private Expression expression;

    public ExFactor(Expression expression) {
        this.expression = expression;
    }

    @Override
    public String toString() {
        return "(" + expression.toString() + ")";
    }

    @Override
    public boolean withSign() {
        return this.toString().charAt(0) == '-' || this.toString().charAt(0) == '+';
    }

    @Override
    public boolean isEmpty() {
        return this.expression == null;
    }

    @Override
    public boolean equals(Object another) {
        return another instanceof ExFactor
                && this.expression.equals(((ExFactor) another).expression);
    }

    @Override
    public boolean canMerge(FactorType another) {
        return false;
    }

    @Override
    public void mulUpdate(FactorType another) {
    }

    @Override
    public ArrayList<FactorType> differential() {
        ArrayList<FactorType> ret = new ArrayList<>();
        Expression inner = this.expression.differential();
        ret.add(new ExFactor(inner));
        return ret;
    }
}
