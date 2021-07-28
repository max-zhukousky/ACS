package com.mycompany.coffeemachine;

/**
 *
 * @author Максим
 */
public class TypesOfCoffee extends CoffeeMachine {

    public TypesOfCoffee(int water, int milk, int coffeeBeans, 
            int money) {
        this.water = water;
        this.milk = milk;
        this.coffeeBeans = coffeeBeans;
        this.money = money;
    }
    
    public boolean checkWater(CoffeeMachine coffeeMachine, 
            TypesOfCoffee typeOfCoffee) {
        return typeOfCoffee.water <= coffeeMachine.water;
    }
    
    public boolean checkMilk(CoffeeMachine coffeeMachine,
            TypesOfCoffee typeOfCoffee) {
        return typeOfCoffee.milk <= coffeeMachine.milk;
    }
    
    public boolean checkCoffeeBeans(CoffeeMachine coffeeMachine,
            TypesOfCoffee typeOfCoffee) {
        return typeOfCoffee.coffeeBeans <= coffeeMachine.coffeeBeans;
    }
    
    public boolean checkMoney(CoffeeMachine coffeeMachine,
            TypesOfCoffee typeOfCoffee) {
        return typeOfCoffee.money <= coffeeMachine.money;
    }
    
}
