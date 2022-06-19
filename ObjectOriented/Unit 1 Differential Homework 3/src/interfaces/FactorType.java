package interfaces;

import java.util.ArrayList;

public interface FactorType {

    @Override
    String toString();

    boolean withSign();

    boolean isEmpty();

    boolean canMerge(FactorType another);

    void mulUpdate(FactorType another);

    ArrayList<FactorType> differential();
}
