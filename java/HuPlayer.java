import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static java.util.stream.Collectors.joining;


public class HuPlayer {
  static String mnem = "";
  static String index = "-1";
  static Map<String, String> codes = new HashMap();

  public static void main(String args[]) {
    File input = new File(args[0]);
    mnem = args[1];
    Document doc = null;
    try {
      codes = loadCountryCodes("/var/www/html/country-hu.dat");
    } catch (Exception e) {
      System.err.println("Error: country codes not loaded." + e.getMessage());
    }
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.magyarfutball.hu");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst("#content");
    extractBio(content);
  }

  private static void extractBio(Element content) {
    Element h2 = content.selectFirst("h2");
    String info = h2.childNode(h2.childNodeSize()-1).toString();
    String parts[] = info.split("\\(");
    String lcob = "";

    String name = parts[0].replaceAll("†", "").trim();
    String nparts[] = name.split(" ");
    String lastName = Arrays.asList(nparts).stream().findFirst().orElse("");
    String firstName = nparts.length > 1 ?
      Arrays.asList(nparts).stream().skip(1).collect(joining(" ")) : "";

    String ob = parts.length > 1 ? parts[1] : "0000-00-00";
    String bparts[] = ob.split(" ");
    int dbi = 0;
    while (dbi < bparts.length && !isDate(bparts[dbi])) dbi++;
    String pob = dbi > 0 ?
        Arrays.asList(bparts).subList(0,dbi).stream().collect(joining(" ")) : " ";
    String dob = dbi < bparts.length ? bparts[dbi] : "0000-00-00";
    String isoDob = toIso(dob);

    Element left = content.selectFirst("#left_col");
    String cob = " ";
    String nat = " ";
    if (left != null) {
      Node nnat = left.selectFirst("strong:contains(nationality)");
      Node natval = nnat.parent().childNode(nnat.siblingIndex()+2);
      String llnat = natval.childNode(0).toString();
      String lnat = llnat.replaceAll("\n", "").trim();
      nat = codes.values().contains(lnat) ? lnat : codes.getOrDefault(lnat, " ");
      cob = codes.values().contains(lcob) ? lcob : codes.getOrDefault(lcob, " ");
    }

    System.out.println(String.format("%s,%s,%s,%s,%s,%s,%s,%s",
        mnem, firstName + " " + lastName, firstName + " " + lastName, isoDob, nat, pob, cob, index));
  }


  private static String toIso(String dob) {
    if (dob==null) return "0000-00-00";
    String dod = dob.split("\\-")[0];
    String[] parts = dod.split("\\.");
    int year = 0;
    int month = 0;
    int day = 0;
    if (parts.length > 0) year = Integer.parseInt(parts[0]);
    if (parts.length > 1) month = Integer.parseInt(parts[1]);
    if (parts.length > 2) day = Integer.parseInt(parts[2]);
    return String.format("%04d-%02d-%02d", year, month, day);
  }

  private static boolean isDate(String s) {
    SimpleDateFormat hudf = new SimpleDateFormat("yyyy.MM.dd");
    SimpleDateFormat incf = new SimpleDateFormat("yyyy");
    try {
      hudf.parse(s);
    } catch (ParseException e1) {
      try {
        incf.parse(s);
      } catch (ParseException e2) {
        return false;
      }
    }
     return true;
  }

  private static Map<String, String> loadCountryCodes(String filename) throws IOException {
    Charset charset = Charset.forName("UTF-8");
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

