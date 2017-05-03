import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.math.BigInteger;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class t08 {

	private static final String NP = "\\p{Zs}*(\\p{Nd}+)\\p{Zs}*";
	private static int c;

	public static void main(String args[]) {

		Pattern p = Pattern.compile(NP);

		Scanner console = null;
		try {
			console = new Scanner(new InputStreamReader(System.in, "UTF-16"));
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}

		String nc = console.nextLine();	// number of cases
		String line;

		while (console.hasNextLine()) {
			line = console.nextLine();
			Matcher m = p.matcher(line);
			System.out.print("Case #" + (++c) + ": ");
			if (m.matches()) {
				BigInteger bi = new BigInteger(m.group(1), 10);
				System.out.println(bi.toString(16));
			} else {
				System.out.println("N/A");
			}
		}
	}
}

