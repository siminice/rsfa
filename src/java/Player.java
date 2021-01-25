import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.time.Month;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;



public class Player {
  static String mnem = "";
  static String index = "-1";
  static Map<String, String> codes = new HashMap();

  public static void main(String args[]) {
    File input = new File(args[0]);
    mnem = args[1];
    Document doc = null;
    try {
      codes = loadCountryCodes("/var/www/html/country-eng.dat");
    } catch (Exception e) {
      System.err.println("Error: country codes not loaded." + e.getMessage());
    }
    try {
      doc = Jsoup.parse(input, "UTF-8", "http://us.soccerway.com/");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Elements maintab = doc.select(".block_player_passport");
    Element tab = maintab.first().selectFirst("dl");
    extractBio(tab);
  }

  private static void extractBio(Element tab) {
    String firstName = findAttribute(tab, "[data-first_name]");
    String lastName = findAttribute(tab, "[data-last_name]");
    String lnat = findAttribute(tab, "[data-nationality]");
    String dob = findAttribute(tab, "[data-date_of_birth]");
    String lcob = findAttribute(tab, "[data-country_of_birth]");
    String pob = findAttribute(tab, "[data-place_of_birth]");
    String isoDob = toIso(dob);
    String nat = codes.values().contains(lnat) ? lnat : codes.getOrDefault(lnat, " ");
    String cob = codes.values().contains(lcob) ? lcob : codes.getOrDefault(lcob, " ");

    System.out.println(String.format("%s,%s,%s,%s,%s,%s,%s,%s",
        mnem, firstName + " " + lastName, firstName + " " + lastName, isoDob, nat, pob, cob, index));
//    System.out.println(String.format("{\n\t'firstName':'%s',\n\t'lastName':'%s',\n\t'nationality':'%s',\n\t'dateOfBirth':'%s',\n\t'countryOfBirth':'%s',\n\t'placeOfBirth':'%s'\n}",
//        firstName, lastName, nat, dob, cob, pob));
  }

  private static String findAttribute(Element e, String s) {
    Element a = e.selectFirst(s);
    if (a == null) return " ";
    Node av = a.childNode(0);
    String val = av.toString().replaceAll("\\n", "");
    if (val.length() == 0) val = " ";
    return val;
  }

  private static String toIso(String dob) {
    if (dob==null) return "0000-00-00";
    String[] parts = dob.split(" ");
    if (parts.length > 1) {
      int year = Integer.parseInt(parts[2]);
      int month = Month.valueOf(parts[1].toUpperCase()).getValue();
      int day = Integer.parseInt(parts[0]);
      return String.format("%04d-%02d-%02d", year, month, day);
    }
    return "0000-00-00";
  }

  private static Map<String, String> loadCountryCodes(String filename) throws IOException {
    Charset charset = Charset.forName("ISO-8859-1");
    List<String> lines = Files.readAllLines(Paths.get(filename), charset);
    Map<String, String> codes = new HashMap();
    for (String s : lines) {
      String[] parts = s.split(",");
      if (parts.length > 1) {
        codes.put(parts[1], parts[0]);
      }
    }
    return codes;
  }
}

