import java.util.HashMap;

public class UnionFindSet {

    private final HashMap<Integer, Integer> elements;
    private final HashMap<Integer, Integer> blocks;

    public UnionFindSet() {
        this.elements = new HashMap<>();
        this.blocks = new HashMap<>();
    }

    public void addElement(int element) {
        int newBlock = elements.size();
        elements.put(element, newBlock);
        blocks.put(newBlock, newBlock);
    }

    public void addLink(int ele1, int ele2) {
        int boss1 = elements.get(ele1);
        int boss2 = elements.get(ele2);
        if (boss1 != boss2) {
            for (Integer ele : elements.keySet()) {
                if (elements.get(ele) == boss1) {
                    elements.replace(ele, boss2);
                }
            }
            blocks.remove(boss1);
        }
    }

    public boolean isLinked(int ele1, int ele2) {
        return elements.get(ele1).equals(elements.get(ele2));
    }

    public int blockSum() {
        return blocks.size();
    }
}
