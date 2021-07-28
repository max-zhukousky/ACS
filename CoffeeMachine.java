package com.mycompany.coffeemachine;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;

/**
 *
 * @author Максим
 */


public class CoffeeMachine {
    
    protected int water;
    protected int milk;
    protected int coffeeBeans;
    protected int disposableCups;
    protected int money;
    // private static CoffeeMachineState coffeeMachineState;
 
    private static final int WATER_FOR_ONE_CUP = 200;
    private static final int MILK_FOR_ONE_CUP = 50;
    private static final int COFFEE_BEANS_FOR_ONE_CUP = 15;
    
    private static final int INITIAL_WATER = 400;
    private static final int INITIAL_MILK = 540;
    private static final int INITIAL_COFFEE_BEANS = 120;
    private static final int INITIAL_DISPOSABLE_CUPS = 9;
    private static final int INITIAL_MONEY = 550;
    
    public CoffeeMachine() {
        this.water = INITIAL_WATER;
        this.milk = INITIAL_MILK;
        this.coffeeBeans = INITIAL_COFFEE_BEANS;
        this.disposableCups = INITIAL_DISPOSABLE_CUPS;
        this.money = INITIAL_MONEY;
    }
    
//    public CoffeeMachine(CoffeeMachineState coffeeMachineState) {
//        CoffeeMachine.coffeeMachineState = coffeeMachineState;
//    }

    public CoffeeMachine(int cupsOfCoffee) {
        this.water = cupsOfCoffee * WATER_FOR_ONE_CUP;
        this.milk = cupsOfCoffee * MILK_FOR_ONE_CUP;
        this.coffeeBeans = cupsOfCoffee * COFFEE_BEANS_FOR_ONE_CUP;
    }
    
    public CoffeeMachine(int water, int milk, int coffeeBeans, int disposableCups,
            int money) {
        this.water = water;
        this.milk = milk;
        this.coffeeBeans = coffeeBeans;
        this.disposableCups = disposableCups;
        this.money = money;
    }
    
    public void calcSuppliesAfterAdding(CoffeeMachine coffeeMachine, int water,
            int milk, int coffeeBeans, int disposableCups) {
        coffeeMachine.water += water;
        coffeeMachine.milk += milk;
        coffeeMachine.coffeeBeans += coffeeBeans;
        coffeeMachine.disposableCups += disposableCups;
    }
    
    public void calcSuppliesWithMilk(CoffeeMachine coffeeMachine, int water,
            int milk, int coffeeBeans, int money) {
        coffeeMachine.water -= water;
        coffeeMachine.milk -= milk;
        coffeeMachine.coffeeBeans -= coffeeBeans;
        coffeeMachine.money += money;
        coffeeMachine.disposableCups -= 1;
    }
    
    public void calcSuppliesWithoutMilk(CoffeeMachine coffeeMachine, int water,
            int coffeeBeans, int money) {
        coffeeMachine.water -= water;
        coffeeMachine.coffeeBeans -= coffeeBeans;
        coffeeMachine.money += money;
        coffeeMachine.disposableCups -= 1;
    }
    
    public void showSupplies(CoffeeMachine coffeeMachine) {
        System.out.println("The coffee machine has: ");
        System.out.println(coffeeMachine.water + " ml of water");
        System.out.println(coffeeMachine.milk + " ml of milk");
        System.out.println(coffeeMachine.coffeeBeans + " g of coffee beans");
        System.out.println(coffeeMachine.disposableCups + " disposable cups");
        System.out.println(coffeeMachine.money + "$ of money");
    }
    
    public void withdrawingMoney(CoffeeMachine coffeeMachine) {
        coffeeMachine.money = 0;
    }
    
    public static boolean executeActions(CoffeeMachine coffeeMachine, String action, 
            List<TypesOfCoffee> typesOfCoffeeList) {
        switch (action) {
                case "buy" -> { 
                    System.out.println("What do you want to buy? 1 - espresso, "
                            + "2 - latte, 3 - cappuccino, back - to main menu: ");
                    Scanner buyOptionScanner = new Scanner(System.in);
                    String buyOption = buyOptionScanner.next();
                    if (buyOption.equals("back")) {
                        break;
                    }
                    HashMap<Integer, TypesOfCoffee> typeOfCoffeeMap = new HashMap<>();
                    typeOfCoffeeMap.put(1, typesOfCoffeeList.get(0));
                    typeOfCoffeeMap.put(2, typesOfCoffeeList.get(1));
                    typeOfCoffeeMap.put(3, typesOfCoffeeList.get(2));
                    TypesOfCoffee typeOfCoffee = typeOfCoffeeMap.get(Integer.valueOf(buyOption));
                    if (typeOfCoffee.checkWater(coffeeMachine, typeOfCoffee) &&
                                    typeOfCoffee.checkMilk(coffeeMachine, typeOfCoffee) &&
                                    typeOfCoffee.checkCoffeeBeans(coffeeMachine, typeOfCoffee) &&
                                    typeOfCoffee.checkMoney(coffeeMachine, typeOfCoffee)) {
                        System.out.println("I have enough resources, making you a coffee!");
                        switch (Integer.valueOf(buyOption)) {
                            case 1 -> {
                                coffeeMachine.calcSuppliesWithoutMilk(coffeeMachine,
                                        250, 16, 4);
                            }
                            case 2 -> {
                                coffeeMachine.calcSuppliesWithMilk(coffeeMachine,
                                        350, 75, 20, 7);
                            }
                            case 3 -> {
                                coffeeMachine.calcSuppliesWithMilk(coffeeMachine,
                                        200, 100, 12, 6);
                            }
                            default -> {
                                System.out.println("No such option!");
                            }
                        }
                    }
                    else if (!typeOfCoffee.checkWater(coffeeMachine, typeOfCoffee)) {
                        System.out.println("Sorry, not enough water!");
                    }
                    else if (!typeOfCoffee.checkMilk(coffeeMachine, typeOfCoffee)) {
                        System.out.println("Sorry, not enough milk!");
                    }
                    else if (!typeOfCoffee.checkCoffeeBeans(coffeeMachine, typeOfCoffee)) {
                        System.out.println("Sorry, not enough coffee beans!");
                    }
                    else if (!typeOfCoffee.checkMoney(coffeeMachine, typeOfCoffee)) {
                        System.out.println("Sorry, not enough money!");
                    }
                    else {
                        System.out.println("Sorry, we just don't like you!");
                    }                
                }
                case "fill" -> {
                    System.out.println("Write how many ml of water you want to add: ");
                    Scanner addWaterSupplyScanner = new Scanner(System.in);
                    int addWater = addWaterSupplyScanner.nextInt();
                    System.out.println("Write how many ml of milk you want to add: ");
                    Scanner addMilkScanner = new Scanner(System.in);
                    int addMilk = addMilkScanner.nextInt();
                    System.out.println("Write how many grams of coffee beans you want to add: ");
                    Scanner addCoffeeBeansScanner = new Scanner(System.in);
                    int addCoffeeBeans = addCoffeeBeansScanner.nextInt();
                    System.out.println("Write how many disposable cups of coffee you want to add: ");
                    Scanner addDisposableCupsOfCoffeeScanner = new Scanner(System.in);
                    Integer addDisposableCupsOfCoffee = addDisposableCupsOfCoffeeScanner.nextInt();
                    coffeeMachine.calcSuppliesAfterAdding(coffeeMachine, addWater, addMilk,
                            addCoffeeBeans, addDisposableCupsOfCoffee);
                }
                case "take" -> {
                    System.out.println("I gave you $" + coffeeMachine.money);
                    coffeeMachine.withdrawingMoney(coffeeMachine);
                }
                case "remaining" -> {
                    coffeeMachine.showSupplies(coffeeMachine);
                }
                case "exit" -> {
                    return false;
                }
                default -> {
                    System.out.println("No such option!");
                    return false;
                }  
            }
        return true;
    }
    
    public static void main(String[] args) {
        TypesOfCoffee espresso = new TypesOfCoffee(250, 0, 16, 4);
        TypesOfCoffee latte = new TypesOfCoffee(200, 100, 12, 6);
        TypesOfCoffee cappuccino = new TypesOfCoffee(350, 75, 20, 7);
        List<TypesOfCoffee> typesOfCoffeeList = new ArrayList<>();
        typesOfCoffeeList.add(espresso);
        typesOfCoffeeList.add(latte);
        typesOfCoffeeList.add(cappuccino);
        CoffeeMachine coffeeMachine = new CoffeeMachine();

        boolean exit = true;
        while (exit) {
            System.out.println("Write action (buy, fill, take, remaining, exit): ");
            Scanner actionScanner = new Scanner(System.in);
            String action = actionScanner.nextLine();
            exit = executeActions(coffeeMachine, action, typesOfCoffeeList);
        }
    }
}