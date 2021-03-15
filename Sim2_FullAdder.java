/* Simulates a physical full adder. Uses two half adders as well as an OR
 * gate in order to represent the full adder operation with a carryIn bit.
 * Call execute() to get the output of the full adder.
 *
 * Author: Jacob Scott
 */
public class Sim2_FullAdder {

	/* 
	 * Runs the full adder on the inputs
	 * 
	 * input: a, b, carryIn
	 * 
	 * output: sum, carryOut
	 */
	public void execute() {
		halfOne.a.set(a.get());
		halfOne.b.set(b.get());
		
		halfOne.execute();
		
		halfTwo.a.set(carryIn.get());
		halfTwo.b.set(halfOne.sum.get());
		
		halfTwo.execute();
		
		or.a.set(halfTwo.carry.get());
		or.b.set(halfOne.carry.get());
		
		or.execute();
		
		sum.set(halfTwo.sum.get());
		carryOut.set(or.out.get());
	}
	
	// inputs
	public RussWire a,b,carryIn;
	// outputs
	public RussWire sum, carryOut;
		
	// internal components
	public Sim2_HalfAdder halfOne;
	public Sim2_HalfAdder halfTwo;
	public OR or;
		
	/* 
	 * Constructor for the full adder
	 */
	public Sim2_FullAdder() {
		a   = new RussWire();
		b   = new RussWire();
		carryIn = new RussWire();

		sum = new RussWire();
		carryOut = new RussWire();

		or = new OR();
		halfOne = new Sim2_HalfAdder();
		halfTwo = new Sim2_HalfAdder();
	}
}
