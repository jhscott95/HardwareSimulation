/* Simulates a physical multi-bit ripple adder. Uses an array of variable length in order to
 * perform the addition on any amount of bits. Also uses an XOR gate in order to calculate
 * if overflow occurred.
 *
 * Author: Jacob Scott
 */
public class Sim2_AdderX {

	/* 
	 * Runs the multi-bit adder on the inputs
	 * 
	 * input: a[], b[]
	 * 
	 * output: sum[], carryOut, overflow
	 */
	public void execute() {
		// Calculates first bit with carryIn set to false because it's a full adder performing addition
		adders[0].carryIn.set(false);
		adders[0].a.set(a[0].get());
		adders[0].b.set(b[0].get());
		
		adders[0].execute();
		sum[0].set(adders[0].sum.get());
		
		for (int i = 1; i < adders.length; i++) {
			adders[i].a.set(a[i].get());
			adders[i].b.set(b[i].get());
			adders[i].carryIn.set(adders[i-1].carryOut.get());
			
			adders[i].execute();
			
			sum[i].set(adders[i].sum.get());
		}
		carryOut.set(adders[adders.length - 1].carryOut.get());
		
		// If the last two carry out bits are different, overflow occurred
		xor.a.set(adders[adders.length - 1].carryOut.get());
		xor.b.set(adders[adders.length - 2].carryOut.get());
		
		xor.execute();
		
		overflow.set(xor.out.get());
	}
	
	// inputs
	public RussWire[] a,b;
	// outputs
	public RussWire[] sum;
	public RussWire   carryOut, overflow;
			
	// internal components
	public Sim2_FullAdder[] adders;
	public Sim2_XOR xor;
	
	/* 
	 * Constructor for the multi-bit ripple adder
	 */
	public Sim2_AdderX(int x) {
		a   = new RussWire[x];
		b   = new RussWire[x];
		sum = new RussWire[x];
		adders = new Sim2_FullAdder[x];

		for (int i = 0; i < x; i++)
		{
			a     [i] = new RussWire();
			b     [i] = new RussWire();
			sum   [i] = new RussWire();
			adders[i] = new Sim2_FullAdder();
		}

		carryOut = new RussWire();
		overflow = new RussWire();
		
		xor = new Sim2_XOR();
	}
}
