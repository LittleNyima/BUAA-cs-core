import com.oocourse.uml3.interact.common.AttributeClassInformation;
import com.oocourse.uml3.interact.common.AttributeQueryType;
import com.oocourse.uml3.interact.common.OperationQueryType;
import com.oocourse.uml3.interact.exceptions.user.AttributeDuplicatedException;
import com.oocourse.uml3.interact.exceptions.user.AttributeNotFoundException;
import com.oocourse.uml3.interact.exceptions.user.ClassDuplicatedException;
import com.oocourse.uml3.interact.exceptions.user.ClassNotFoundException;
import com.oocourse.uml3.interact.exceptions.user.ConflictQueryTypeException;
import com.oocourse.uml3.interact.exceptions.user.InteractionDuplicatedException;
import com.oocourse.uml3.interact.exceptions.user.InteractionNotFoundException;
import com.oocourse.uml3.interact.exceptions.user.LifelineDuplicatedException;
import com.oocourse.uml3.interact.exceptions.user.LifelineNotFoundException;
import com.oocourse.uml3.interact.exceptions.user.StateDuplicatedException;
import com.oocourse.uml3.interact.exceptions.user.StateMachineDuplicatedException;
import com.oocourse.uml3.interact.exceptions.user.StateMachineNotFoundException;
import com.oocourse.uml3.interact.exceptions.user.StateNotFoundException;
import com.oocourse.uml3.interact.exceptions.user.UmlRule001Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule002Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule003Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule004Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule005Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule006Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule007Exception;
import com.oocourse.uml3.interact.exceptions.user.UmlRule008Exception;
import com.oocourse.uml3.interact.format.UmlGeneralInteraction;
import com.oocourse.uml3.models.common.ElementType;
import com.oocourse.uml3.models.common.Visibility;
import com.oocourse.uml3.models.elements.UmlAssociation;
import com.oocourse.uml3.models.elements.UmlAssociationEnd;
import com.oocourse.uml3.models.elements.UmlAttribute;
import com.oocourse.uml3.models.elements.UmlElement;
import com.oocourse.uml3.models.elements.UmlGeneralization;
import com.oocourse.uml3.models.elements.UmlInterfaceRealization;
import com.oocourse.uml3.models.elements.UmlMessage;
import com.oocourse.uml3.models.elements.UmlParameter;
import com.oocourse.uml3.models.elements.UmlTransition;
import common.MyOperationQueryType;
import elements.MyUmlClass;
import elements.MyUmlElement;
import elements.MyUmlInteraction;
import elements.MyUmlInterface;
import elements.MyUmlLifeline;
import elements.MyUmlOperation;
import elements.MyUmlOthers;
import elements.MyUmlState;
import elements.MyUmlStateMachine;
import interfaces.Generalizable;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MyUmlGeneralInteraction implements UmlGeneralInteraction {

    private final ArrayList<MyUmlElement> elementList;
    private final HashMap<String, MyUmlElement> idElementMap;
    private final HashMap<String, ArrayList<MyUmlClass>> nameClassMap;
    private final HashMap<String, ArrayList<MyUmlStateMachine>> nameStateMachineMap;
    private final HashMap<String, ArrayList<MyUmlInteraction>> nameInteractionMap;
    private int classCount;

    public MyUmlGeneralInteraction(UmlElement... elements) {
        this.elementList = new ArrayList<>();
        this.idElementMap = new HashMap<>();
        this.nameClassMap = new HashMap<>();
        this.nameStateMachineMap = new HashMap<>();
        this.nameInteractionMap = new HashMap<>();
        this.classCount = 0;
        this.initObjects(elements);
        this.initAttributes(elements);
        this.initDlc(elements);
    }

    private void initObjects(UmlElement... elements) {
        for (UmlElement element : elements) {
            try {
                ElementType type = element.getElementType();
                if (type == ElementType.UML_CLASS) {
                    MyUmlClass myUmlClass = new MyUmlClass(element);
                    idElementMap.put(element.getId(), myUmlClass);
                    classCount++;
                    if (!nameClassMap.containsKey(element.getName())) {
                        ArrayList<MyUmlClass> list = new ArrayList<>();
                        list.add(myUmlClass);
                        nameClassMap.put(element.getName(), list);
                    } else {
                        nameClassMap.get(element.getName()).add(myUmlClass);
                    }
                } else if (type == ElementType.UML_INTERFACE) {
                    idElementMap.put(element.getId(), new MyUmlInterface(element));
                } else if (type == ElementType.UML_OPERATION) {
                    idElementMap.put(element.getId(), new MyUmlOperation(element));
                } else if (type == ElementType.UML_STATE_MACHINE) {
                    MyUmlStateMachine myUmlStateMachine = new MyUmlStateMachine(element);
                    idElementMap.put(element.getId(), myUmlStateMachine);
                    if (!nameStateMachineMap.containsKey(element.getName())) {
                        ArrayList<MyUmlStateMachine> list = new ArrayList<>();
                        list.add(myUmlStateMachine);
                        nameStateMachineMap.put(element.getName(), list);
                    } else {
                        nameStateMachineMap.get(element.getName()).add(myUmlStateMachine);
                    }
                } else if (type == ElementType.UML_INTERACTION) {
                    MyUmlInteraction myUmlInteraction = new MyUmlInteraction(element);
                    idElementMap.put(element.getId(), myUmlInteraction);
                    if (!nameInteractionMap.containsKey(element.getName())) {
                        ArrayList<MyUmlInteraction> list = new ArrayList<>();
                        list.add(myUmlInteraction);
                        nameInteractionMap.put(element.getName(), list);
                    } else {
                        nameInteractionMap.get(element.getName()).add(myUmlInteraction);
                    }
                } else if (type == ElementType.UML_STATE ||
                        type == ElementType.UML_PSEUDOSTATE ||
                        type == ElementType.UML_FINAL_STATE) {
                    idElementMap.put(element.getId(), new MyUmlState(element));
                } else if (type == ElementType.UML_LIFELINE) {
                    idElementMap.put(element.getId(), new MyUmlLifeline(element));
                } else {
                    idElementMap.put(element.getId(), new MyUmlOthers(element));
                }
                MyUmlElement myUmlElement = idElementMap.get(element.getId());
                if (myUmlElement != null) {
                    elementList.add(myUmlElement);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private void initAttributes(UmlElement... elements) {
        ArrayList<UmlElement> paramList = new ArrayList<>();
        for (UmlElement element : elements) {
            try {
                ElementType type = element.getElementType();
                if (type == ElementType.UML_ASSOCIATION) {
                    UmlAssociationEnd end1 = (UmlAssociationEnd) idElementMap
                            .get(((UmlAssociation) element).getEnd1()).getUmlElement();
                    UmlAssociationEnd end2 = (UmlAssociationEnd) idElementMap
                            .get(((UmlAssociation) element).getEnd2()).getUmlElement();
                    MyUmlElement myUmlElement1 = idElementMap.get(end1.getReference());
                    MyUmlElement myUmlElement2 = idElementMap.get(end2.getReference());
                    if (myUmlElement1.getElementType() == ElementType.UML_CLASS) {
                        ((MyUmlClass) myUmlElement1).addAssociation(myUmlElement2);
                        ((MyUmlClass) myUmlElement1).addAssociationEnd(end2);
                    }
                    if (myUmlElement2.getElementType() == ElementType.UML_CLASS) {
                        ((MyUmlClass) myUmlElement2).addAssociation(myUmlElement1);
                        ((MyUmlClass) myUmlElement2).addAssociationEnd(end1);
                    }
                } else if (type == ElementType.UML_ATTRIBUTE) {
                    if (idElementMap.containsKey(element.getParentId())) {
                        MyUmlElement myUmlElement = idElementMap.get(element.getParentId());
                        if (myUmlElement.getElementType() == ElementType.UML_CLASS) {
                            ((MyUmlClass) myUmlElement).addAttribute((UmlAttribute) element);
                        } else if (myUmlElement.getElementType() == ElementType.UML_INTERFACE) {
                            ((MyUmlInterface) myUmlElement).addAttribute((UmlAttribute) element);
                        }
                    }
                } else if (type == ElementType.UML_GENERALIZATION) {
                    ((Generalizable) idElementMap.get(((UmlGeneralization) element).getSource()))
                            .addParent((Generalizable) idElementMap
                                    .get(((UmlGeneralization) element).getTarget()));
                } else if (type == ElementType.UML_INTERFACE_REALIZATION) {
                    ((MyUmlClass) idElementMap.get(((UmlInterfaceRealization) element).getSource()))
                            .implementInterface((MyUmlInterface) idElementMap
                                    .get(((UmlInterfaceRealization) element).getTarget()));
                } else if (type == ElementType.UML_OPERATION) {
                    MyUmlElement myUmlElement = idElementMap.get(element.getParentId());
                    if (myUmlElement.getElementType() == ElementType.UML_CLASS) {
                        ((MyUmlClass) myUmlElement)
                                .addOperation((MyUmlOperation) idElementMap.get(element.getId()));
                    } else if (myUmlElement.getElementType() == ElementType.UML_INTERFACE) {
                        ((MyUmlInterface) myUmlElement)
                                .addOperation((MyUmlOperation) idElementMap.get(element.getId()));
                    }
                } else if (type == ElementType.UML_PARAMETER) {
                    paramList.add(element);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        for (UmlElement element : paramList) {
            ((MyUmlOperation) idElementMap.get(element.getParentId()))
                    .addParam((UmlParameter) element);
        }
    }

    private void initDlc(UmlElement... elements) {
        for (UmlElement element : elements) {
            try {
                ElementType type = element.getElementType();
                if (type == ElementType.UML_STATE ||
                        type == ElementType.UML_FINAL_STATE ||
                        type == ElementType.UML_PSEUDOSTATE) {
                    MyUmlElement region = idElementMap.get(element.getParentId());
                    MyUmlElement stateMachine = idElementMap.get(region.getParentId());
                    MyUmlElement state = idElementMap.get(element.getId());
                    ((MyUmlStateMachine) stateMachine).addState((MyUmlState) state);
                } else if (type == ElementType.UML_TRANSITION) {
                    MyUmlElement region = idElementMap.get(element.getParentId());
                    MyUmlElement source = idElementMap.get(((UmlTransition) element).getSource());
                    MyUmlElement target = idElementMap.get(((UmlTransition) element).getTarget());
                    MyUmlElement stateMachine = idElementMap.get(region.getParentId());
                    ((MyUmlStateMachine) stateMachine).addTransition();
                    ((MyUmlState) source).addSubsequence((MyUmlState) target);
                } else if (type == ElementType.UML_LIFELINE) {
                    MyUmlElement interaction = idElementMap.get(element.getParentId());
                    MyUmlElement participant = idElementMap.get(element.getId());
                    ((MyUmlInteraction) interaction).addParticipant((MyUmlLifeline) participant);
                } else if (type == ElementType.UML_MESSAGE) {
                    MyUmlElement interaction = idElementMap.get(element.getParentId());
                    MyUmlElement target = idElementMap.get(((UmlMessage) element).getTarget());
                    ((MyUmlInteraction) interaction).addMessage();
                    if (target.getElementType() == ElementType.UML_LIFELINE) {
                        ((MyUmlLifeline) target).addIncomingMessage();
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private MyUmlClass findClassByName(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        if (!nameClassMap.containsKey(className)) {
            throw new ClassNotFoundException(className);
        } else {
            ArrayList<MyUmlClass> list = nameClassMap.get(className);
            if (list.size() != 1) {
                throw new ClassDuplicatedException(className);
            } else {
                return list.get(0);
            }
        }
    }

    private MyUmlStateMachine findStateMachineByName(String stateMachineName)
            throws StateMachineNotFoundException, StateMachineDuplicatedException {
        if (!nameStateMachineMap.containsKey(stateMachineName)) {
            throw new StateMachineNotFoundException(stateMachineName);
        } else {
            ArrayList<MyUmlStateMachine> list = nameStateMachineMap.get(stateMachineName);
            if (list.size() != 1) {
                throw new StateMachineDuplicatedException(stateMachineName);
            } else {
                return list.get(0);
            }
        }
    }

    private MyUmlInteraction findInteractionByName(String interactionName)
            throws InteractionNotFoundException, InteractionDuplicatedException {
        if (!nameInteractionMap.containsKey(interactionName)) {
            throw new InteractionNotFoundException(interactionName);
        } else {
            ArrayList<MyUmlInteraction> list = nameInteractionMap.get(interactionName);
            if (list.size() != 1) {
                throw new InteractionDuplicatedException(interactionName);
            } else {
                return list.get(0);
            }
        }
    }

    @Override
    public int getClassCount() {
        return this.classCount;
    }

    @Override
    public int getClassOperationCount(String className, OperationQueryType[] queryTypes)
            throws ClassNotFoundException, ClassDuplicatedException, ConflictQueryTypeException {
        MyUmlClass myUmlClass = findClassByName(className);
        MyOperationQueryType type = sortQueryTypes(queryTypes);
        return myUmlClass.getClassOperationCount(type);
    }

    private MyOperationQueryType sortQueryTypes(OperationQueryType[] queryTypes)
            throws ConflictQueryTypeException {
        Boolean hasParam = null;
        Boolean hasReturn = null;
        for (OperationQueryType query : queryTypes) {
            switch (query) {
                case PARAM:
                    if (hasParam != null && !hasParam) {
                        throw new ConflictQueryTypeException();
                    } else {
                        hasParam = true;
                    }
                    break;
                case NON_PARAM:
                    if (hasParam != null && hasParam) {
                        throw new ConflictQueryTypeException();
                    } else {
                        hasParam = false;
                    }
                    break;
                case RETURN:
                    if (hasReturn != null && !hasReturn) {
                        throw new ConflictQueryTypeException();
                    } else {
                        hasReturn = true;
                    }
                    break;
                case NON_RETURN:
                    if (hasReturn != null && hasReturn) {
                        throw new ConflictQueryTypeException();
                    } else {
                        hasReturn = false;
                    }
                    break;
                default:
            }
        }
        if (hasParam == null && hasReturn == null) {
            return MyOperationQueryType.ALL;
        } else if (hasParam == null) {
            if (hasReturn) {
                return MyOperationQueryType.BOTH_RET;
            } else {
                return MyOperationQueryType.BOTH_NO_RET;
            }
        } else if (hasReturn == null) {
            if (hasParam) {
                return MyOperationQueryType.PARAM_BOTH;
            } else {
                return MyOperationQueryType.NO_PARAM_BOTH;
            }
        } else if (hasParam && hasReturn) {
            return MyOperationQueryType.PARAM_RET;
        } else if (hasParam) {
            return MyOperationQueryType.PARAM_NO_RET;
        } else if (hasReturn) {
            return MyOperationQueryType.NO_PARAM_RET;
        } else {
            return MyOperationQueryType.NO_PARAM_NO_RET;
        }
    }

    @Override
    public int getClassAttributeCount(String className, AttributeQueryType queryType)
            throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getClassAttributeCount(queryType);
    }

    @Override
    public int getClassAssociationCount(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getClassAssociationCount();
    }

    @Override
    public List<String> getClassAssociatedClassList(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getClassAssociatedClassList();
    }

    @Override
    public Map<Visibility, Integer> getClassOperationVisibility(
            String className, String operationName
    ) throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getClassOperationVisibility(operationName);
    }

    @Override
    public Visibility getClassAttributeVisibility(String className, String attributeName)
            throws ClassNotFoundException, ClassDuplicatedException,
            AttributeNotFoundException, AttributeDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getClassAttributeVisibility(attributeName);
    }

    @Override
    public String getTopParentClass(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getTopParentClass();
    }

    @Override
    public List<String> getImplementInterfaceList(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getImplementInterfaceList();
    }

    @Override
    public List<AttributeClassInformation> getInformationNotHidden(String className)
            throws ClassNotFoundException, ClassDuplicatedException {
        MyUmlClass myUmlClass = findClassByName(className);
        return myUmlClass.getInformationNotHidden();
    }

    @Override
    public int getStateCount(String stateMachineName)
            throws StateMachineNotFoundException, StateMachineDuplicatedException {
        MyUmlStateMachine myUmlStateMachine = findStateMachineByName(stateMachineName);
        return myUmlStateMachine.getStateCount();
    }

    @Override
    public int getTransitionCount(String stateMachineName)
            throws StateMachineNotFoundException, StateMachineDuplicatedException {
        MyUmlStateMachine myUmlStateMachine = findStateMachineByName(stateMachineName);
        return myUmlStateMachine.getTransitionCount();
    }

    @Override
    public int getSubsequentStateCount(String stateMachineName, String stateName)
            throws StateMachineNotFoundException, StateMachineDuplicatedException,
            StateNotFoundException, StateDuplicatedException {
        MyUmlStateMachine myUmlStateMachine = findStateMachineByName(stateMachineName);
        return myUmlStateMachine.getSubsequentStateCount(stateName);
    }

    @Override
    public int getParticipantCount(String interactionName)
            throws InteractionNotFoundException, InteractionDuplicatedException {
        MyUmlInteraction myUmlInteraction = findInteractionByName(interactionName);
        return myUmlInteraction.getParticipantCount();
    }

    @Override
    public int getMessageCount(String interactionName)
            throws InteractionNotFoundException, InteractionDuplicatedException {
        MyUmlInteraction myUmlInteraction = findInteractionByName(interactionName);
        return myUmlInteraction.getMessageCount();
    }

    @Override
    public int getIncomingMessageCount(String interactionName, String lifelineName)
            throws InteractionNotFoundException, InteractionDuplicatedException,
            LifelineNotFoundException, LifelineDuplicatedException {
        MyUmlInteraction myUmlInteraction = findInteractionByName(interactionName);
        return myUmlInteraction.getIncomingMessageCount(lifelineName);
    }

    @Override
    public void checkForUml001() throws UmlRule001Exception {
        MyUmlChecker.checkForUml001(elementList);
    }

    @Override
    public void checkForUml002() throws UmlRule002Exception {
        MyUmlChecker.checkForUml002(elementList);
    }

    @Override
    public void checkForUml003() throws UmlRule003Exception {
        MyUmlChecker.checkForUml003(elementList);
    }

    @Override
    public void checkForUml004() throws UmlRule004Exception {
        MyUmlChecker.checkForUml004(elementList);
    }

    @Override
    public void checkForUml005() throws UmlRule005Exception {
        MyUmlChecker.checkForUml005(elementList, idElementMap);
    }

    @Override
    public void checkForUml006() throws UmlRule006Exception {
        MyUmlChecker.checkForUml006(elementList);
    }

    @Override
    public void checkForUml007() throws UmlRule007Exception {
        MyUmlChecker.checkForUml007(elementList);
    }

    @Override
    public void checkForUml008() throws UmlRule008Exception {
        MyUmlChecker.checkForUml008(elementList);
    }
}
