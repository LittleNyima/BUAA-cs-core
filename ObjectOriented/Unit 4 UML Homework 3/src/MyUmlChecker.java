import com.oocourse.uml3.interact.common.AttributeClassInformation;
import com.oocourse.uml3.interact.exceptions.user.UmlRule001Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule002Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule003Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule004Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule005Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule006Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule007Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule008Exception;
import com.oocourse.uml3.models.common.Direction;
import com.oocourse.uml3.models.common.ElementType;
import com.oocourse.uml3.models.elements.UmlClassOrInterface;
import com.oocourse.uml3.models.elements.UmlParameter;
import elements.MyUmlClass;
import elements.MyUmlClassOrInterface;
import elements.MyUmlElement;
import elements.MyUmlInterface;
import elements.MyUmlState;

import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;

public class MyUmlChecker {

    private MyUmlChecker() {
    }

    public static void checkForUml001(List<MyUmlElement> list) throws UmlRule001Exception {
        HashSet<AttributeClassInformation> pairs = new HashSet<>();
        for (MyUmlElement element : list) {
            if (element.getElementType() == ElementType.UML_CLASS) {
                HashSet<String> names = new HashSet<>();
                for (String name : ((MyUmlClass) element).getNameList()) {
                    if (name != null) {
                        if (!names.contains(name)) {
                            names.add(name);
                        } else {
                            pairs.add(new AttributeClassInformation(name, element.getName()));
                        }
                    }
                }
            }
        }
        if (!pairs.isEmpty()) {
            throw new UmlRule001Exception(pairs);
        }
    }

    public static void checkForUml002(List<MyUmlElement> list) throws UmlRule002Exception {
        HashSet<UmlClassOrInterface> classOrInterfaces = new HashSet<>();
        for (MyUmlElement element : list) {
            if (element.getElementType() == ElementType.UML_CLASS) {
                MyUmlClass myUmlClass = (MyUmlClass) element;
                HashSet<MyUmlClass> flag = new HashSet<>();
                while (myUmlClass.getParentClass() != null) {
                    if (element.equals(myUmlClass.getParentClass())) {
                        classOrInterfaces.add((UmlClassOrInterface) element.getUmlElement());
                        break;
                    } else if (flag.contains(myUmlClass)) {
                        break;
                    }
                    flag.add(myUmlClass);
                    myUmlClass = myUmlClass.getParentClass();
                }
            } else if (element.getElementType() == ElementType.UML_INTERFACE) {
                Queue<MyUmlInterface> queue = new LinkedList<>();
                HashSet<MyUmlInterface> flag = new HashSet<>();
                queue.offer((MyUmlInterface) element);
                flag.add((MyUmlInterface) element);
                while (!queue.isEmpty()) {
                    MyUmlInterface myUmlInterface = queue.poll();
                    for (MyUmlInterface parent : myUmlInterface.getParents()) {
                        if (parent.equals(element)) {
                            classOrInterfaces.add((UmlClassOrInterface) element.getUmlElement());
                            queue = new LinkedList<>();
                            break;
                        } else if (!flag.contains(parent)) {
                            queue.offer(parent);
                            flag.add(parent);
                        }
                    }
                }
            }
        }
        if (!classOrInterfaces.isEmpty()) {
            throw new UmlRule002Exception(classOrInterfaces);
        }
    }

    public static void checkForUml003(List<MyUmlElement> list) throws UmlRule003Exception {
        HashSet<UmlClassOrInterface> classOrInterfaces = new HashSet<>();
        for (MyUmlElement element : list) {
            if (element.getElementType() == ElementType.UML_INTERFACE) {
                Queue<MyUmlInterface> queue = new LinkedList<>();
                HashSet<MyUmlInterface> flag = new HashSet<>();
                queue.offer((MyUmlInterface) element);
                flag.add((MyUmlInterface) element);
                while (!queue.isEmpty()) {
                    MyUmlInterface myUmlInterface = queue.poll();
                    for (MyUmlInterface parent : myUmlInterface.getParents()) {
                        if (flag.contains(parent)) {
                            classOrInterfaces.add((UmlClassOrInterface) element.getUmlElement());
                            queue = new LinkedList<>();
                            break;
                        } else {
                            queue.offer(parent);
                            flag.add(parent);
                        }
                    }
                }
            }
        }
        if (!classOrInterfaces.isEmpty()) {
            throw new UmlRule003Exception(classOrInterfaces);
        }
    }

    public static void checkForUml004(List<MyUmlElement> list) throws UmlRule004Exception {
        HashSet<UmlClassOrInterface> classOrInterfaces = new HashSet<>();
        for (MyUmlElement element : list) {
            if (element.getElementType() == ElementType.UML_CLASS) {
                Queue<MyUmlClassOrInterface> queue = new LinkedList<>();
                HashSet<MyUmlClassOrInterface> flag = new HashSet<>();
                queue.offer((MyUmlClassOrInterface) element);
                flag.add((MyUmlClassOrInterface) element);
                while (!queue.isEmpty()) {
                    MyUmlClassOrInterface ci = queue.poll();
                    for (MyUmlClassOrInterface parent : ci.getParentsClassOrInterface()) {
                        if (!flag.contains(parent)) {
                            queue.offer(parent);
                            flag.add(parent);
                        } else if (parent.getElementType() == ElementType.UML_INTERFACE) {
                            classOrInterfaces.add((UmlClassOrInterface) element.getUmlElement());
                            queue = new LinkedList<>();
                            break;
                        }
                    }
                }
            }
        }
        if (!classOrInterfaces.isEmpty()) {
            throw new UmlRule004Exception(classOrInterfaces);
        }
    }

    public static void checkForUml005(List<MyUmlElement> list, Map<String, MyUmlElement> map)
            throws UmlRule005Exception {
        for (MyUmlElement element : list) {
            ElementType type = element.getElementType();
            if (type == ElementType.UML_CLASS ||
                    type == ElementType.UML_INTERFACE ||
                    type == ElementType.UML_OPERATION) {
                if (element.getName() == null) {
                    throw new UmlRule005Exception();
                }
            } else if (type == ElementType.UML_PARAMETER &&
                    ((UmlParameter) element.getUmlElement()).getDirection() != Direction.RETURN) {
                if (element.getName() == null) {
                    throw new UmlRule005Exception();
                }
            } else if (type == ElementType.UML_ATTRIBUTE &&
                    map.containsKey(element.getParentId())) {
                ElementType parentType = map.get(element.getParentId()).getElementType();
                if (parentType == ElementType.UML_CLASS ||
                        parentType == ElementType.UML_INTERFACE) {
                    if (element.getName() == null) {
                        throw new UmlRule005Exception();
                    }
                }
            }
        }
    }

    public static void checkForUml006(List<MyUmlElement> list) throws UmlRule006Exception {
        for (MyUmlElement element : list) {
            if (element.getElementType() == ElementType.UML_INTERFACE) {
                if (!((MyUmlInterface) element).allAttributeIsPublic()) {
                    throw new UmlRule006Exception();
                }
            }
        }
    }

    public static void checkForUml007(List<MyUmlElement> list) throws UmlRule007Exception {
        for (MyUmlElement element : list) {
            if (element.getElementType() == ElementType.UML_FINAL_STATE) {
                if (((MyUmlState) element).getDirectSubsequentStateCount() != 0) {
                    throw new UmlRule007Exception();
                }
            }
        }
    }

    public static void checkForUml008(List<MyUmlElement> list) throws UmlRule008Exception {
        for (MyUmlElement element : list) {
            if (element.getElementType() == ElementType.UML_PSEUDOSTATE) {
                if (((MyUmlState) element).getDirectSubsequentStateCount() > 1) {
                    throw new UmlRule008Exception();
                }
            }
        }
    }
}
